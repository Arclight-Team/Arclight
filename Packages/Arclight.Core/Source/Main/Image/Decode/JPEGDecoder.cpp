/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 JPEGDecoder.cpp
 */

#include "Image/Decode/JPEGDecoder.hpp"
#include "Common/Intrinsic.hpp"
#include "Util/Log.hpp"
#include "Time/Profiler.hpp"
#include "Util/Bool.hpp"
#include "Common/Assert.hpp"
#include "Common/Exception.hpp"

#include <map>

#include ARC_INTRINSIC_H



using namespace JPEG;


constexpr static u32 fixScaleShift = 10;
constexpr static u32 fixMultiplyShift = 10;
constexpr static u32 fixTransformShift = 7;
constexpr static u32 ycbcrShift = 14;

template<u32 Shift>
constexpr static i32 colorBias = (Shift && Shift < 32) ? Bits::ones<i32>(Shift - 1) : 0;


constexpr static i32 coeffBaseDifference[16] = {
	0, -1, -3, -7, -15, -31, -63, -127, -255, -511, -1023, -2047, -4095, -8191, -16383, -32767
};

constexpr static i32 entropyPositiveBase[16] = {
	0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384
};


constexpr static std::array<i32, 64> scaleFactors = []() {

	std::array<i32, 64> a {};

	constexpr long double x0 = 0.3535533905932737622004221810524245196424179688442370182941699344L; //1 / (2 * sqrt(2))
	constexpr long double x1 = 0.4499881115682078523192547704709441977690008637064224926177235580L; //cos(7 * pi / 16) / (2 * sin(3 * pi / 8) - sqrt(2))
	constexpr long double x2 = 0.6532814824381882639283215867135935767918805941746347637744491834L; //cos(pi / 8) / sqrt(2)
	constexpr long double x3 = 0.2548977895520795844709699019939219568413092459544676848632214685L; //cos(5 * pi / 16) / (2 * cos(3 * pi / 8) + sqrt(2))
	constexpr long double x4 = 0.3535533905932737622004221810524245196424179688442370182941699344L; //1 / (2 * sqrt(2))
	constexpr long double x5 = 1.2814577238707530893980431480888499545075616756936724560638481482L; //cos(3 * pi / 16) / (-2 * cos(3 * pi / 8) + sqrt(2))
	constexpr long double x6 = 0.2705980500730984921998616026831947100305360316890077223406485478L; //cos(3 * pi / 8) / sqrt(2)
	constexpr long double x7 = 0.3006724434675226402718609119546109175336279448003363610609320596L; //cos(pi / 16) / (2 * sin(3 * pi / 8) + sqrt(2))

	constexpr long double scale[64] = {
		x0 * x0, x0 * x1, x0 * x2, x0 * x3, x0 * x4, x0 * x5, x0 * x6, x0 * x7,
		x1 * x0, x1 * x1, x1 * x2, x1 * x3, x1 * x4, x1 * x5, x1 * x6, x1 * x7,
		x2 * x0, x2 * x1, x2 * x2, x2 * x3, x2 * x4, x2 * x5, x2 * x6, x2 * x7,
		x3 * x0, x3 * x1, x3 * x2, x3 * x3, x3 * x4, x3 * x5, x3 * x6, x3 * x7,
		x4 * x0, x4 * x1, x4 * x2, x4 * x3, x4 * x4, x4 * x5, x4 * x6, x4 * x7,
		x5 * x0, x5 * x1, x5 * x2, x5 * x3, x5 * x4, x5 * x5, x5 * x6, x5 * x7,
		x6 * x0, x6 * x1, x6 * x2, x6 * x3, x6 * x4, x6 * x5, x6 * x6, x6 * x7,
		x7 * x0, x7 * x1, x7 * x2, x7 * x3, x7 * x4, x7 * x5, x7 * x6, x7 * x7,
	};

	for (u32 i = 0; i < 64; i++) {
		a[i] = i32(scale[i] * (1 << fixScaleShift) + 0.5);
	}

	return a;

}();

constexpr static std::array<i32, 3> multiplyConstants = []() {

	std::array<i32, 3> a {};

	constexpr long double x[3] = {
		0.7071067811865475244008443621048490392848359376884740365883398689L,    //cos(pi / 4)
		0.3826834323650897717284599840303988667613445624856270414338006356L,    //cos(3 * pi / 8)
		0.9238795325112867561281831893967882868224166258636424861150977312L     //sin(3 * pi / 8)
	};

	for (u32 i = 0; i < 3; i++) {
		a[i] = i32(x[i] * (1 << fixMultiplyShift) + 0.5);
	}

	return a;

}();

constexpr static std::array<i16, 4> ycbcrFactors = []() {

	std::array<i16, 4> a {};

	constexpr long double x[4] = {
		1.402L,
		0.3441136L,
		0.714136L,
		1.772L
	};

	for (u32 i = 0; i < 4; i++) {
		a[i] = i16(x[i] * (1 << ycbcrShift) + 0.5);
	}

	return a;

}();



constexpr static bool app0StringCompare(const u8* a, const u8* b) noexcept {

	while (true) {

		u8 x = *a++;
		u8 y = *b++;

		if (x != y) {
			return false;
		} else if (!x) {
			return true;
		}

	}

}



void JPEGDecoder::decode(std::span<const u8> data) {

	validDecode = false;

	reader = BinaryReader(data, ByteOrder::Big);


	//The file must at least fit SOI / EOI marker
	if (reader.remainingSize() < 4) {
		throw ImageDecoderException("Data size too small");
	}

	//Start by looking for SOI
	u16 soiMarker = reader.read<u16>();

	if (soiMarker != Markers::SOI) {
		throw ImageDecoderException("No SOI marker found");
	}

	restartEnabled = false;


	//Find interpret markers
	u16 marker = reader.read<u16>();

	while (marker != Markers::EOI) {

		switch (marker) {

			case Markers::APP0:
				parseApplicationSegment0();
				break;

			case Markers::APP1:
				parseApplicationSegment1();
				break;

			case Markers::DQT:
				parseQuantizationTable();
				break;

			case Markers::DHT:
				parseHuffmanTable();
				break;

			case Markers::DAC:
				parseArithmeticConditioning();
				break;

			case Markers::COM:
				parseComment();
				break;

			case Markers::DRI:
				parseRestartInterval();
				break;

			case Markers::SOF0:
			case Markers::SOF1:
			case Markers::SOF2:
			case Markers::SOF3:
			case Markers::SOF5:
			case Markers::SOF6:
			case Markers::SOF7:
			case Markers::JPG:
			case Markers::SOF9:
			case Markers::SOF10:
			case Markers::SOF11:
			case Markers::SOF13:
			case Markers::SOF14:
			case Markers::SOF15:

				frame.type = static_cast<FrameType>(marker & 0x03);
				frame.encoding = static_cast<Encoding>((marker & 0x08) >> 3);
				frame.differential = marker & 0x04;

				parseFrameHeader();
				break;

			case Markers::SOS:
				{
					parseScanHeader();
					resolveTargetFormat();
					decodeScan();
				}
				break;

			default:
				//LogE("JPEG Decoder").print("Unknown marker 0x%X", marker);
				reader.seek(-1);
				break;

		}


		//Next marker
		if (reader.remainingSize() < 2) {
			throw ImageDecoderException("Bad stream end");
		}

		marker = reader.read<u16>();

	}

	blendAndUpsample();

	validDecode = true;

}



RawImage& JPEGDecoder::getImage() {

	if (!validDecode) {
		throw ImageDecoderException("Bad image decode");
	}

	return image;

}



void JPEGDecoder::parseApplicationSegment0() {

	u16 length = verifySegmentLength();
	SizeT segmentEnd = reader.position() + length;

	const u8* app0String = reader.head();
	bool stringValid = false;

	for (u32 i = 0; i < length - 2; i++) {

		if (reader.read<u8>() == 0x00) {
			stringValid = true;
			break;
		}

	}

	if (!stringValid) {
		throw ImageDecoderException("Bad APP0 type string");
	}

	if (app0StringCompare(app0String, jfifString)) {

		//JFIF APP0
		if (length < 16) {
			throw ImageDecoderException("Bad APP0 segment length of " + std::to_string(length));
		}

		u8 major = reader.read<u8>();
		u8 minor = reader.read<u8>();
		u8 density = reader.read<u8>();
		u16 densityX = reader.read<u16>();
		u16 densityY = reader.read<u16>();
		u8 thumbnailW = reader.read<u8>();
		u8 thumbnailH = reader.read<u8>();

#ifdef ARC_CFG_IMAGE_DEBUG
		LogI("JPEG Decoder").print("[APP0 JFIF] Major: %d, Minor: %d, Density: 0x%X, DensityX: %d, DensityY: %d, ThumbnailW: %d, ThumbnailH: %d",
				  major, minor, density, densityX, densityY, thumbnailW, thumbnailH);
#endif

		if (density > 2 || !densityX || !densityY) {
			throw ImageDecoderException("Illegal APP0 segment data");
		}

		u32 thumbnailBytes = thumbnailW * thumbnailH * 3;

		if (length != thumbnailBytes + 16) {
			throw ImageDecoderException("Bad APP0 length of " + std::to_string(length));
		}

		Image<Pixel::RGB8> thumbnail;
		thumbnail.setRawData(std::span {reader.head(), thumbnailBytes});

	} else if (app0StringCompare(app0String, jfxxString)) {

		//JFXX APP0
		//Shared + SOI / Thumbnail dims
		if (length < 8 + 2) {
			throw ImageDecoderException("Bad APP0 extension segment length of " + std::to_string(length));
		}

		u8 format = reader.read<u8>();

#ifdef ARC_CFG_IMAGE_DEBUG
		LogI("JPEG Decoder").print("[APP0 JFXX] ThumbnailFormat: 0x%X", format);
#endif

		switch (format) {

			case 0x10:
				//TODO: Embedded JPEG
				LogW("JPEG Decoder") << "Embedded JPEG thumbnails unsupported";
				reader.seekTo(segmentEnd);
				break;

			case 0x11:
			case 0x13:
				{
					u8 thumbnailW = reader.read<u8>();
					u8 thumbnailH = reader.read<u8>();
					u32 thumbnailPixels = thumbnailW * thumbnailH;

					Image<Pixel::RGB8> thumbnail(thumbnailW, thumbnailH);

					if (format == 0x11) {

						//256-color palette in RGB8
						if (length != thumbnailPixels + 768 + 10) {
							throw ImageDecoderException("Bad APP0 extension length of " + std::to_string(length));
						}

						const u8* palette = reader.head();
						reader.seek(768);

						for (u32 y = 0; y < thumbnailH; y++) {

							for (u32 x = 0; x < thumbnailW; x++) {

								u8 index = reader.read<u8>();
								const u8* paletteEntry = palette + index * 3;

								thumbnail.setPixel(x, y, PixelRGB8(paletteEntry[0], paletteEntry[1], paletteEntry[2]));

							}

						}

					} else {

						//Uncompressed RGB8
						if (length != thumbnailPixels * 3 + 10) {
							throw ImageDecoderException("Bad APP0 extension length of " + std::to_string(length));
						}

						thumbnail.setRawData(std::span {reader.head(), thumbnailPixels * 3});

					}

				}
				break;

			default:
				throw ImageDecoderException("Bad APP0 extension thumbnail format");

		}

	} else {

		//Unknown, skip
		LogW("JPEG Decoder").print("Unknown APP0 segment found: %s", app0String);
		reader.seekTo(segmentEnd);

	}

}



void JPEGDecoder::parseApplicationSegment1() {

	u16 length = verifySegmentLength();
	reader.seek(length - 2);

	//TODO: Parse Exif segment

}



void JPEGDecoder::parseHuffmanTable() {

	u16 length = verifySegmentLength();

	//Length
	u32 offset = 2;
	u32 count = 0;

	do {

		//Setting + L1-16
		offset += 17;

		if (length < offset) {
			throw ImageDecoderException("[DHT] Bad table length");
		}

		u8 settings = reader.read<u8>();
		u8 type = settings >> 4;
		u8 id = settings & 0xF;
		bool dc = type == 0;

#ifdef ARC_CFG_IMAGE_DEBUG
		LogI("JPEG Decoder").print("[DHT] Class: %s, ID: %d", dc ? "DC" : "AC", id);
#endif

		if (type > 1 || id > 3) {
			throw ImageDecoderException("[DHT] Bad table settings");
		}

		std::array<u8, 16> codeCounts{};
		u32 totalCodeCount = 0;
		u32 highestLength = 0;

		for (u32 i = 0; i < 16; i++) {

			u8 codeCount = reader.read<u8>();

			codeCounts[i] = codeCount;
			totalCodeCount += codeCount;

			if (codeCount) {
				highestLength = i + 1;
			}

		}

		offset += totalCodeCount;

		if (length < offset) {
			throw ImageDecoderException("[DHT] Bad table length");
		}

		HuffmanTable& table = dc ? dcHuffmanTables[id] : acHuffmanTables[id];

		if (!table.empty()) {
			table.reset();
		}

		table.maxLength = highestLength;

		std::vector<u8> symbols(totalCodeCount);
		reader.read(std::span {symbols.data(), symbols.size()});

		if (dc) {

			//DC symbols can be greater than 0xB (illegally)
			//While we can handle up to 0xF, everything above is not covered and needs to be caught
			for (u8& v : symbols) {

				if (v > 0xF) {

					LogW("JPEG Decoder").print("Illegal DC symbol 0x%02X, overriding", v);
					v = 0x0;

				}

			}

		}

		u32 index = 0;
		u32 code = 0;
		u32 extLength = highestLength - 8;
		bool extGenerated = false;

		std::map<u8, u8> extMap;

		//For each code length
		for (u32 i = 0; i < highestLength; i++) {

			//For each code
			for (u32 j = 0; j < codeCounts[i]; j++, code++, index++) {

				if (i < 8) {

					//Fill FHT
					u32 startCode = code << (7 - i);
					u32 endCode = startCode + (1 << (7 - i));

					for (u32 k = startCode; k < endCode; k++) {
						table.fastTable[k] = {symbols[index], i + 1};
					}

				} else {

					if (!extGenerated) {

						//Initialize EHTs
						for (u32 k = 0; k < table.fastTable.size(); k++) {

							if (table.fastTable[k].first == HuffmanTable::defaultHuffmanResult.first) {

								u8 tableIndex = table.extTables.size();

								table.fastTable[k] = {tableIndex, 0xFF};
								extMap.try_emplace(k, tableIndex);
								table.extTables.emplace_back(std::vector<HuffmanResult>(2 << extLength, HuffmanResult {0, 1}));

							}

						}

						extGenerated = true;

					}

					//Fill EHT
					u32 fastPrefix = code >> (i - 7);
					u32 extCode = Bits::mask(code, 0, i - 7);

					u32 startCode = extCode << (extLength + 7 - i);
					u32 endCode = startCode + (1 << (extLength + 7 - i));

					for (u32 k = startCode; k < endCode; k++) {
						table.extTables[extMap[fastPrefix]][k] = {symbols[index], i + 1};
					}

				}

			}

			code <<= 1;

		}

		count++;

	} while (offset != length);

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[DHT] Tables read: %d", count);
#endif

}



void JPEGDecoder::parseArithmeticConditioning() {

	u16 length = verifySegmentLength();

	//Length
	u32 offset = 2;
	u32 count = 0;

	do {

		//Setting + Cs
		offset += 2;

		if (length < offset) {
			throw ImageDecoderException("[DAC] Bad table length");
		}

		u8 settings = reader.read<u8>();
		u8 type = settings >> 4;
		u8 id = settings & 0xF;
		bool dc = type == 0;

#ifdef ARC_CFG_IMAGE_DEBUG
		LogI("JPEG Decoder").print("[DAC] Class: %s, ID: %d", dc ? "DC" : "AC", id);
#endif

		if (type > 1 || id > 3) {
			throw ImageDecoderException("[DAC] Bad table settings");
		}

		u8 cs = reader.read<u8>();

		if (dc) {

			u8 u = cs >> 4;
			u8 l = cs & 0xF;

			if (l > u) {
				throw ImageDecoderException("[DAC] Bad DC conditioning");
			}

			ArithmeticDCConditioning& dcc = dcConditioning[id];
			dcc.lowerBound = l == 0 ? 0 : 1 << (l - 1);
			dcc.upperBound = 1 << u;
			dcc.active = true;

#ifdef ARC_CFG_IMAGE_DEBUG
			LogI("JPEG Decoder").print("[DAC] LowerBound: %d, UpperBound: %d", dcc.lowerBound, dcc.upperBound);
#endif

		} else {

			if (cs == 0 || cs > 63) {
				throw ImageDecoderException("[DAC] Bad AC conditioning");
			}

			ArithmeticACConditioning& acc = acConditioning[id];
			acc.kx = cs;
			acc.active = true;

#ifdef ARC_CFG_IMAGE_DEBUG
			LogI("JPEG Decoder").print("[DAC] Kx: %d", acc.kx);
#endif

		}

		count++;

	} while (offset != length);

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[DAC] Tables read: %d", count);
#endif

}



void JPEGDecoder::parseQuantizationTable() {

	u16 length = verifySegmentLength();

	u32 offset = 2;
	u32 count = 0;

	do {

		if (length < offset + 1) {
			throw ImageDecoderException("[DQT] Bad table length");
		}

		u8 setting = reader.read<u8>();
		u8 precision = setting >> 4;
		u8 id = setting & 0xF;

#ifdef ARC_CFG_IMAGE_DEBUG
		LogI("JPEG Decoder").print(std::string("[DQT] Table %d, ") + (precision ? "16 bit" : "8 bit"), id);
#endif

		if (precision > 1 || id > 3) {
			throw ImageDecoderException("[DQT] Illegal quantization setting");
		}

		offset += 65 + precision * 64;

		if (length < offset) {
			throw ImageDecoderException("[DQT] Table too small");
		}

		auto loadQTable = [this](u32 tableID, auto size) {

			using T = decltype(size);

			QuantizationTable& table = quantizationTables[tableID];
			table.hasData = true;

			for (u32 i = 0; i < 64; i++) {
				table.data[i] = static_cast<i32>(reader.read<T>()) * scaleFactors[dezigzagTable[i]];
			}

		};

		if (precision) {
			loadQTable(id, u16{});
		} else {
			loadQTable(id, u8{});
		}

		count++;

	} while (offset != length);

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[DQT] Tables read: %d", count);
#endif

}



void JPEGDecoder::parseRestartInterval() {

	u16 length = verifySegmentLength();

	if (length != 4) {
		throw ImageDecoderException("[DRI] Bad table size");
	}

	restartInterval = reader.read<u16>();
	restartEnabled = restartInterval;

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[DRI] RestartInterval: %d", restartInterval);
#endif

}



void JPEGDecoder::parseComment() {

	u16 length = verifySegmentLength();

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[COM] Comment: " + std::string(reinterpret_cast<const char*>(reader.head()), length - 2));
#endif

	comment.assign(reinterpret_cast<const char8_t*>(reader.head()), length - 2);

}



void JPEGDecoder::parseNumberOfLines() {

	u16 length = verifySegmentLength();

	if (length != 4) {
		throw ImageDecoderException("[DNL] Bad table size");
	}

	u16 lines = reader.read<u16>();

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[DNL] Lines: %d", lines);
#endif

	if (!lines) {
		throw ImageDecoderException("[DNL] Line count cannot be 0");
	}

	frame.lines = lines;

}



void JPEGDecoder::parseFrameHeader() {

	u16 length = verifySegmentLength();

	if (length < 8) {
		throw ImageDecoderException("[SOF] Bad length");
	}

	//TODO: If lines == 0, DNL after first scan
	frame.bits = reader.read<u8>();
	frame.lines = reader.read<u16>();
	frame.samples = reader.read<u16>();

	u8 components = reader.read<u8>();

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[SOF] Bits: %d, Lines: %d, Samples: %d, Components: %d", frame.bits, frame.lines, frame.samples, components);
#endif

	switch (frame.type) {

		case FrameType::Sequential:

			if (frame.bits != 8) {
				throw ImageDecoderException("[SOF] Bit depth for baseline must be 8");
			}

			break;

		case FrameType::ExtendedSequential:

			if (frame.bits != 8 && frame.bits != 12) {
				throw ImageDecoderException("[SOF] Bit depth for extended sequential must be 8 or 12");
			}

			break;

		case FrameType::Progressive:

			if (frame.bits != 8 && frame.bits != 12) {
				throw ImageDecoderException("[SOF] Bit depth for progressive must be 8 or 12");
			}

			if (components > 4) {
				throw ImageDecoderException("[SOF] Progressive cannot have more than 4 components");
			}

			break;

		case FrameType::Lossless:

			if (!Math::inRange(frame.bits, 2, 16)) {
				throw ImageDecoderException("[SOF] Bit depth for lossless must be between 2 and 16");
			}

			break;

	}

	if (components == 0) {
		throw ImageDecoderException("[SOF] Component count may not be 0");
	}

	if (frame.samples == 0) {
		throw ImageDecoderException("[SOF] Line sample count may not be 0");
	}

	if (length < components * 3 + 8) {
		throw ImageDecoderException("[SOF] Bad length");
	}

	for (u32 i = 0; i < components; i++) {

		u8 id = reader.read<u8>();
		u8 s = reader.read<u8>();
		u8 q = reader.read<u8>();

		FrameComponent component(s >> 4, s & 0xF, q);

#ifdef ARC_CFG_IMAGE_DEBUG
		LogI("JPEG Decoder").print("[SOF] Component: %d, SX: %d, SY: %d, QTableIndex: %d", id, component.samplesX, component.samplesY, q);
#endif

		if (!Math::inRange(component.samplesX, 1, 4) || !Math::inRange(component.samplesY, 1, 4) || q > 3 || (frame.type == FrameType::Lossless && q != 0)) {
			throw ImageDecoderException("[SOF] Bad component data");
		}

		if (frame.components.contains(id)) {
			throw ImageDecoderException("[SOF] Duplicate component ID");
		}

		frame.components.emplace(id, component);

	}

	//Find DNL if lines are zero
	if (!frame.lines) {
		searchForLineSegment();
	}

}



void JPEGDecoder::parseScanHeader() {

	u16 length = verifySegmentLength();

	if (length < 3) {
		throw ImageDecoderException("[SOS] Bad length");
	}

	u8 components = reader.read<u8>();

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[SOS] Images: %d", components);
#endif

	if (!Math::inRange(components, 1, 4)) {
		throw ImageDecoderException("[SOS] Illegal component count");
	}

	if (length < components * 2 + 6) {
		throw ImageDecoderException("[SOS] Bad length");
	}

	for (u32 i = 0; i < components; i++) {

		u8 componentID = reader.read<u8>();

		u8 ids = reader.read<u8>();
		u8 dcTableID = ids >> 4;
		u8 acTableID = ids & 0xF;

#ifdef ARC_CFG_IMAGE_DEBUG
		LogI("JPEG Decoder").print("[SOS] Component %d: ID: %d, DCID: %d, ACID: %d", i + 1, componentID, dcTableID, acTableID);
#endif

		if (!frame.components.contains(componentID)) {
			throw ImageDecoderException("[SOS] No frame component found matching scan component index");
		}

		FrameComponent& frameComponent = frame.components[componentID];

		scan.maxSamplesX = Math::max(scan.maxSamplesX, frameComponent.samplesX);
		scan.maxSamplesY = Math::max(scan.maxSamplesY, frameComponent.samplesY);

		if (dcTableID > 3 || (frame.type == FrameType::Sequential && dcTableID > 1)) {
			throw ImageDecoderException("[SOS] Illegal DC table ID");
		}

		if (acTableID > 3 || (frame.type == FrameType::Sequential && acTableID > 1) || (frame.type == FrameType::Lossless && acTableID)) {
			throw ImageDecoderException("[SOS] Illegal AC table ID");
		}

		if (frame.type != FrameType::Progressive) {

			bool incompleteDCTables = dcHuffmanTables[dcTableID].empty() || (frame.type != FrameType::Lossless && acHuffmanTables[acTableID].empty());

			if (frame.encoding == Encoding::Huffman) {

				if (dcHuffmanTables[dcTableID].empty() || (frame.type != FrameType::Lossless && acHuffmanTables[acTableID].empty())) {
					throw ImageDecoderException("[SOS] Huffman table not installed");
				}

			} else {

				if (!dcConditioning[dcTableID].active || (frame.type != FrameType::Lossless && !acConditioning[acTableID].active)) {
					throw ImageDecoderException("[SOS] Arithmetic coder not conditioned");
				}

			}

		}

		if (frame.type != FrameType::Lossless) {

			if (quantizationTables[frameComponent.qID].empty()) {
				throw ImageDecoderException("[SOS] Quantization table not installed");
			}

		}

		scan.scanComponents.emplace_back(i, dcHuffmanTables[dcTableID], acHuffmanTables[acTableID], dcConditioning[dcTableID], acConditioning[acTableID], quantizationTables[frameComponent.qID], frameComponent);

	}

	scan.spectralStart = reader.read<u8>();
	scan.spectralEnd = reader.read<u8>();

	u8 approx = reader.read<u8>();
	scan.approximationHigh = approx >> 4;
	scan.approximationLow = approx & 0xF;

#ifdef ARC_CFG_IMAGE_DEBUG
	LogI("JPEG Decoder").print("[SOS] SStart: %d, SEnd: %d, ApproxHigh: %d, ApproxLow: %d", scan.spectralStart, scan.spectralEnd, scan.approximationHigh, scan.approximationLow);
#endif

	switch (frame.type) {

		case FrameType::Sequential:
		case FrameType::ExtendedSequential:

			if (scan.spectralStart != 0 || scan.spectralEnd != 63 || scan.approximationHigh != 0 || scan.approximationLow != 0) {
				throw ImageDecoderException("[SOS] Bad scan settings");
			}

			break;

		case FrameType::Progressive:

			if (scan.spectralStart > 63 || scan.spectralEnd > 63 || scan.spectralStart > scan.spectralEnd
				|| (!scan.spectralStart && scan.spectralEnd) || scan.approximationHigh > 13 || scan.approximationLow > 13) {
				throw ImageDecoderException("[SOS] Bad scan settings");
			}

			if (scan.spectralStart != 0 && components > 1) {
				throw ImageDecoderException("[SOS] Progressive AC scan can only cover one component at a time");
			}

			if (scan.spectralStart == 0) {

				for (const ScanComponent& component : scan.scanComponents) {

					if (component.dcTable.empty()) {
						throw ImageDecoderException("[SOS] Progressive DC huffman table is empty");
					}

				}

			} else {

				if (scan.scanComponents[0].acTable.empty()) {
					throw ImageDecoderException("[SOS] Progressive AC huffman table is empty");
				}

			}

			break;

		case FrameType::Lossless:
			{
				if (scan.spectralStart > 7 || scan.spectralEnd || scan.approximationHigh || scan.approximationLow > 15) {
					throw ImageDecoderException("[SOS] Bad scan settings");
				}

				if (restartEnabled && !(restartInterval % scan.mcusX)) {
					throw ImageDecoderException("Restart interval must be an integer multiple of row MCUs");
				}

				//Alias to properly named variables
				scan.pointTransform = scan.approximationLow;
				scan.predictor = scan.spectralStart;

				//Pre-calculate initial prediction
				if (frame.bits - 1 < scan.pointTransform) {

					LogW("JPEG Decoder") << "Specified point transform rounded to 0";
					scan.pointTransform = 0;

				}
			}
			break;

	}

	bool calcMCUs = true;
	scan.mcuDataUnits = 0;

	//Setup component data
	for (ScanComponent& scanComponent : scan.scanComponents) {

		FrameComponent& frameComponent = scanComponent.frameComponent;

		frameComponent.width = (frame.samples * frameComponent.samplesX + scan.maxSamplesX - 1) / scan.maxSamplesX;
		frameComponent.height = (frame.lines * frameComponent.samplesY + scan.maxSamplesY - 1) / scan.maxSamplesY;

		u32 multipleX = frameComponent.samplesX;
		u32 multipleY = frameComponent.samplesY;

		if (frame.type != FrameType::Lossless) {

			multipleX *= 8;
			multipleY *= 8;

		}

		scan.mcuDataUnits += frameComponent.samplesX * frameComponent.samplesY;

		if (calcMCUs) {

			scan.mcusX = (frameComponent.width + multipleX - 1) / multipleX;
			scan.mcusY = (frameComponent.height + multipleY - 1) / multipleY;
			scan.totalMCUs = scan.mcusX * scan.mcusY;
			calcMCUs = false;

		}

		if (frame.type == FrameType::Progressive) {
			frameComponent.progressiveBuffer.resize(scan.mcusX * scan.mcusY * 64);
		}

		frameComponent.imageData.resize(frameComponent.width * frameComponent.height);

	}

	if (scan.mcuDataUnits > 10) {
		throw ImageDecoderException("Too many data units in MCU");
	}

}



void JPEGDecoder::searchForLineSegment() {

	SizeT pos = reader.position();

	//Assume it's somewhere in the file
	while (reader.remainingSize() >= 2) {

		if (reader.read<u8>() == (Markers::DNL >> 8) && reader.read<u8>() == (Markers::DNL & 0xFF)) {

			parseNumberOfLines();
			reader.seekTo(pos);
			return;

		}

	}

	throw ImageDecoderException("No DNL segment found in file");

}



void JPEGDecoder::resolveTargetFormat() {

	//TODO: Finish this list
	if (autoDetectFormat()) {

		switch (scan.scanComponents.size()) {

			case 1:
				baseFormat = Pixel::Grayscale8;
				break;

			case 3:
				baseFormat = Pixel::RGB8;
				break;

			default:
				LogD() << "Unknown format";
				break;

		}

	} else {

		Pixel format = requestedFormat.value();

		switch (scan.scanComponents.size()) {

			case 1:
				baseFormat = Pixel::Grayscale8;
				break;

			case 3:
				baseFormat = Pixel::RGB8;
				break;

			default:
				LogD() << "Unknown format";
				break;

		}

	}

}



void JPEGDecoder::decodeScan() {

	if (!reader.size()) {
		throw ImageDecoderException("Scan empty");
	}

	if (frame.bits != 8 || frame.type == FrameType::Progressive || frame.differential || (frame.type == FrameType::Lossless && frame.encoding == Encoding::Arithmetic)) {
		throw UnsupportedOperationException("JPEG cannot be progressive, hierarchical or non-8 bpp");
	}

	//Start scan decoding
	if (restartEnabled) {

		u32 restartCount = scan.totalMCUs / restartInterval;
		u32 baseMCU = 0;

		for (u32 i = 0; i < restartCount; i++) {

			decodeImage(baseMCU, baseMCU + restartInterval);
			baseMCU += restartInterval;

			//Skip potential marker
			if (reader.remainingSize() >= 2 && Math::inRange(reader.peek<u16>(), Markers::RST0, Markers::RST7)) {
				reader.seek(2);
			}

		}

		if (scan.totalMCUs % restartInterval) {
			decodeImage(baseMCU, scan.totalMCUs);
		}

	} else {

		decodeImage(0, scan.totalMCUs);

	}

}



void JPEGDecoder::decodeImage(u32 startMCU, u32 endMCU) {

	auto decodeBlock = [this](ScanComponent& scanComponent, bool Huffman) constexpr {

		if (Huffman) {
			decodeHuffmanBlock(scanComponent);
		} else {
			decodeArithmeticBlock(scanComponent);
		}

	};

	auto doDecode = [&, this]<FrameType Type, bool Huffman, bool Interleave>() {

		SizeT mcuX = startMCU % scan.mcusX;
		SizeT mcuY = startMCU / scan.mcusX;

		auto sequentialDecode = [&](ScanComponent& scanComponent) {

			FrameComponent& component = scanComponent.frameComponent;

			SizeT mcuBaseX = mcuX * component.samplesX * 8;
			SizeT mcuBaseY = mcuY * component.samplesY * 8;

			for (u32 sy = 0; sy < component.samplesY; sy++) {

				SizeT baseY = mcuBaseY + sy * 8;

				if (baseY + 8 > component.height) {

					SizeT h = component.height - baseY;

					for (u32 sx = 0; sx < component.samplesX; sx++) {

						SizeT baseX = mcuBaseX + sx * 8;
						SizeT w = 8;

						if (baseX + 8 > component.width) {
							w = component.width - baseX;
						}

						decodeBlock(scanComponent, Huffman);
						applyPartialIDCT(scanComponent, baseY * component.width + baseX, w, h);

					}

				} else {

					for (u32 sx = 0; sx < component.samplesX; sx++) {

						SizeT baseX = mcuBaseX + sx * 8;

						decodeBlock(scanComponent, Huffman);

						if (baseX + 8 > component.width) {
							applyPartialIDCT(scanComponent, baseY * component.width + baseX, component.width - baseX, 8);
						} else {
							applyIDCT(scanComponent, baseY * component.width + baseX);
						}

					}

				}

			}

		};

		auto progressiveDecode = [&](ScanComponent& component) {};

		auto losslessDecode = [&, this](ScanComponent& scanComponent) {

			FrameComponent& component = scanComponent.frameComponent;

			SizeT mcuBaseX = mcuX * component.samplesX;
			SizeT mcuBaseY = mcuY * component.samplesY;

			u32 predictor = scan.predictor;

			for (u32 sy = 0; sy < component.samplesY; sy++) {

				u32 y = mcuBaseY + sy;

				if (y == 0) {
					predictor = 1;
				}

				for (u32 sx = 0; sx < component.samplesX; sx++) {

					u32 x = mcuBaseX + sx;

					if (x == 0) {
						predictor = y ? 2 : 0;
					}

					predictSample(scanComponent, x, y, predictor);

				}

			}

		};

		for (u32 currentMCU = startMCU; currentMCU < endMCU; currentMCU++, mcuX++) {

			if (mcuX >= scan.mcusX) {

				mcuX = 0;
				mcuY++;

			}

			for (u32 i = 0; Interleave ? i < scan.scanComponents.size() : i < 1; i++) {

				ScanComponent& component = scan.scanComponents[i];

				if constexpr (Type == FrameType::Sequential || Type == FrameType::ExtendedSequential) {
					sequentialDecode(component);
				} else if constexpr (Type == FrameType::Progressive) {
					progressiveDecode(component);
				} else {
					losslessDecode(component);
				}

			}

		}

	};


	//Reset encoders
	if (frame.encoding == Encoding::Huffman) {

		huffmanDecoder.reset();

	} else {

		arithmeticDecoder.reset();
		arithmeticDecoder.prefetch();

		for (ScanComponent& component : scan.scanComponents) {

			component.dcConditioning.bins.fill({});
			component.acConditioning.bins.fill({});

		}

	}


	if (frame.type == FrameType::Sequential || frame.type == FrameType::ExtendedSequential) {

		alignas(32) i32 block[64];

		//Reset prediction and block buffer
		for (ScanComponent& component : scan.scanComponents) {

			component.prediction = 0;
			component.prevDifference = 0;
			component.block = block;

		}

		if (frame.encoding == Encoding::Huffman) {

			if (scan.scanComponents.size() > 1) {
				doDecode.template operator()<FrameType::Sequential, true, true>();
			} else {
				doDecode.template operator()<FrameType::Sequential, true, false>();
			}

		} else {

			if (scan.scanComponents.size() > 1) {
				doDecode.template operator()<FrameType::Sequential, false, true>();
			} else {
				doDecode.template operator()<FrameType::Sequential, false, false>();
			}

		}

	} else if (frame.type == FrameType::Progressive) {

		bool dcProgression = scan.spectralStart == 0;

		//Reset prediction
		for (ScanComponent& component : scan.scanComponents) {

			FrameComponent& frameComponent = component.frameComponent;

			if (dcProgression && frameComponent.progression != 0) {
				throw ImageDecoderException("DC progression after AC progression");
			}

			component.prediction = 0;

		}

		if (frame.encoding == Encoding::Huffman) {

			if (scan.scanComponents.size() > 1) {
				doDecode.template operator()<FrameType::Progressive, true, true>();
			} else {
				doDecode.template operator()<FrameType::Progressive, true, false>();
			}

		} else {

			if (scan.scanComponents.size() > 1) {
				doDecode.template operator()<FrameType::Progressive, false, true>();
			} else {
				doDecode.template operator()<FrameType::Progressive, false, false>();
			}

		}

	} else {

		if (frame.encoding == Encoding::Huffman) {

			if (scan.scanComponents.size() > 1) {
				doDecode.template operator()<FrameType::Lossless, true, true>();
			} else {
				doDecode.template operator()<FrameType::Lossless, true, false>();
			}

		} else {

			if (scan.scanComponents.size() > 1) {
				doDecode.template operator()<FrameType::Lossless, false, true>();
			} else {
				doDecode.template operator()<FrameType::Lossless, false, false>();
			}

		}

	}

};



void JPEGDecoder::decodeHuffmanBlock(JPEG::ScanComponent& component) {

	i32* block = clearBlockBuffer(component);

	//DC
	{
		HuffmanResult result = huffmanDecoder.decodeDC(component.dcTable);

		u32 category = result.first;
		i32 difference = 0;
		u32 offset = 0;

		if (category) {

			offset = huffmanDecoder.decodeOffset(category);
			difference = offset >= entropyPositiveBase[category] ? static_cast<i32>(offset) : coeffBaseDifference[category] + static_cast<i32>(offset);

		}

		i32 dc = component.prediction + difference;
		component.prediction = dc;
		block[0] = dc * component.qTable.data[0];
	}

	//AC
	u32 coefficient = 1;

	while (coefficient < 64) {

		HuffmanResult result = huffmanDecoder.decodeAC(component.acTable);

		u8 symbol = result.first;
		u8 category = symbol & 0xF;
		u8 zeroes = symbol >> 4;

		if (category == 0) {

			//Special values
			if (zeroes == 0) {

				//End Of Block
				break;

			} else if (zeroes == 0xF) {

				//Zero Run Length
				coefficient += 16;

			} else {

				LogW("JPEG Decoder") << "Bad AC symbol, skipping";
				coefficient++;

			}

		} else {

			//Extract the AC magnitude
			u32 offset = huffmanDecoder.decodeOffset(category);

			coefficient += zeroes;

			//Calculate the AC value
			i32 ac = offset >= entropyPositiveBase[category] ? static_cast<i32>(offset) : coeffBaseDifference[category] + static_cast<i32>(offset);

			if (coefficient >= 64) {

				//Oh no
				LogW("JPEG Decoder") << "AC symbol overflow, stream corrupted";
				break;

			}

			u32 dezigzagIndex = dezigzagTableTransposed[coefficient];
			block[dezigzagIndex] = ac * component.qTable.data[coefficient];

			coefficient++;

		}

	}

}



void JPEGDecoder::decodeArithmeticBlock(JPEG::ScanComponent& component) {

	i32* block = clearBlockBuffer(component);
	i32 difference = 0;

	{
		i32 absPrevDifference = Math::abs(component.prevDifference);
		bool positivePrevDifference = component.prevDifference >= 0;
		u32 baseBin = 0;
		u32 binOffset = 0;

		if (absPrevDifference > component.dcConditioning.lowerBound) {

			if (absPrevDifference > component.dcConditioning.upperBound) {
				baseBin = positivePrevDifference ? 12 : 16;
			} else {
				baseBin = positivePrevDifference ? 4 : 8;
			}

		}

		if (arithmeticDecoder.decodeDCBin(component, baseBin)) {

			//Decode_V
			bool negative = arithmeticDecoder.decodeDCBin(component, baseBin + 1);
			u32 currentBin = baseBin + (negative ? 3 : 2);

			u32 m = 0;

			if (arithmeticDecoder.decodeDCBin(component, currentBin)) {

				m = 1;
				currentBin = 20;

				while (arithmeticDecoder.decodeDCBin(component, currentBin) && currentBin < 34) {

					m <<= 1;
					currentBin += 1;

				}

			}

			u32 value = m;
			currentBin += 14;

			while (m >>= 1) {

				if (arithmeticDecoder.decodeDCBin(component, currentBin)) {
					value |= m;
				}

			}

			difference = static_cast<i32>(value + 1);

			if (negative) {
				difference = -difference;
			}

		}

		i32 dc = component.prediction + difference;
		component.prediction = dc;
		component.prevDifference = difference;
		block[0] = dc * component.qTable.data[0];
	}

	{
		u32 k = 1;
		u32 baseBin = 0;

		while (!arithmeticDecoder.decodeACBin(component, baseBin)) {

			while (!arithmeticDecoder.decodeACBin(component, baseBin + 1)) {

				if (k == 63) {
					LogW("JPEG Decoder") << "Bad AC symbol, skipping";
					return;
				}

				baseBin += 3;
				k++;

			}

			//Decode_V
			bool negative = arithmeticDecoder.decodeFixed(0x5A1D, false);
			u32 currentBin = baseBin + 2;

			u32 m = 0;

			if (arithmeticDecoder.decodeACBin(component, currentBin)) {

				m = 1;

				if (arithmeticDecoder.decodeACBin(component, currentBin)) {

					m = 2;
					currentBin = k <= component.acConditioning.kx ? 189 : 217;
					u32 lastBin = currentBin + 14;

					while (arithmeticDecoder.decodeACBin(component, currentBin) && currentBin < lastBin) {

						m <<= 1;
						currentBin += 1;

					}

				}

			}

			u32 value = m;
			currentBin += 14;

			while (m >>= 1) {

				if (arithmeticDecoder.decodeACBin(component, currentBin)) {
					value |= m;
				}

			}

			i32 ac = static_cast<i32>(value + 1);

			if (negative) {
				ac = -ac;
			}

			u32 dezigzagIndex = dezigzagTableTransposed[k];
			block[dezigzagIndex] = ac * component.qTable.data[k];

			if (k == 63) {
				break;
			}

			k++;
			baseBin += 3;

		}

	}

}



void JPEGDecoder::decodeProgressiveDCBlock(JPEG::ScanComponent& component) {

	i32* block = std::assume_aligned<32>(component.block);

}



void JPEGDecoder::predictSample(JPEG::ScanComponent& component, u32 x, u32 y, u32 predictor) {

	i32 prediction = calculatePrediction(component, x, y, predictor);

	HuffmanResult result = huffmanDecoder.decodeDC(component.dcTable);

	u32 category = result.first;
	i32 difference = 0;

	if (category) {

		if (category < 16) {

			u32 offset = huffmanDecoder.decodeOffset(category);
			difference = offset >= entropyPositiveBase[category] ? static_cast<i32>(offset) : coeffBaseDifference[category] + static_cast<i32>(offset);

		} else {

			difference = 32768;

		}

	}

	component.frameComponent.imageData[y * component.frameComponent.width + x] = static_cast<i16>((prediction + difference) << scan.pointTransform);

}



i32 JPEGDecoder::calculatePrediction(JPEG::ScanComponent& component, u32 x, u32 y, u32 predictor) {

	i32 prediction = 0;

	switch (predictor) {

		case 0:
		default:
			prediction = 1 << (frame.bits - scan.pointTransform - 1);
			break;

		case 1:
			prediction = sampleComponent(component, x - 1, y);
			break;

		case 2:
			prediction = sampleComponent(component, x, y - 1);
			break;

		case 3:
			prediction = sampleComponent(component, x - 1, y - 1);
			break;

		case 4:
			prediction = calculatePrediction(component, x, y, 1) + calculatePrediction(component, x, y, 2) - calculatePrediction(component, x, y, 3);
			break;

		case 5:
			prediction = calculatePrediction(component, x, y, 1) + ((calculatePrediction(component, x, y, 2) - calculatePrediction(component, x, y, 3)) >> 1);
			break;

		case 6:
			prediction = calculatePrediction(component, x, y, 2) + ((calculatePrediction(component, x, y, 1) - calculatePrediction(component, x, y, 3)) >> 1);
			break;

		case 7:
			prediction = ((calculatePrediction(component, x, y, 1) + calculatePrediction(component, x, y, 2)) >> 1);
			break;

	}

	return prediction;

}



i16 JPEGDecoder::sampleComponent(JPEG::ScanComponent& component, u32 x, u32 y) {
	return component.frameComponent.imageData[y * component.frameComponent.width + x];
}



ARC_FORCE_INLINE i32* JPEGDecoder::clearBlockBuffer(JPEG::ScanComponent& component) {

	i32* block = std::assume_aligned<32>(component.block);


	// This really shouldn't be necessary, but MSVC fails to vectorize std::fill_n
	arc_intrinsic_avx2 (

		__m256i* ptr = reinterpret_cast<__m256i*>(block);

		for (u32 i = 0; i < 8; i++) {
			_mm256_store_si256(ptr++, _mm256_setzero_si256());
		}

	) else arc_intrinsic_sse2 (

		__m128i* ptr = reinterpret_cast<__m128i*>(block);

		for (u32 i = 0; i < 16; i++) {
			_mm_store_si128(ptr++, _mm_setzero_si128());
		}

	) else {

		std::fill_n(block, 64, 0);

	}

	return block;

}



template<bool Full>
constexpr static void scalarIDCT(const i32* inData, i16* outData, SizeT outStride, u32 width, u32 height) {

	i32 buffer[64];
	SizeT sizeX = Full ? 8 : width;
	SizeT sizeY = Full ? 8 : height;

	//First pass
	for (u32 i = 0; i < 8; i++) {

		u32 k = i * 8;

		//Stage 1: Pre-multiplication stage
		i32 a0 = inData[k + 0] + inData[k + 4];
		i32 a1 = inData[k + 0] - inData[k + 4];
		i32 a2 = inData[k + 2] - inData[k + 6];
		i32 a3 = inData[k + 2] + inData[k + 6];
		i32 a4 = inData[k + 1] + inData[k + 7];
		i32 a5 = inData[k + 1] - inData[k + 7];
		i32 a6 = inData[k + 5] - inData[k + 3];
		i32 a7 = inData[k + 5] + inData[k + 3];
		i32 a8 = a6 - a4;
		i32 a9 = a6 + a4;

		//Stage 2: Multiplication
		i32 b0 = (a3 * multiplyConstants[0]) >> fixMultiplyShift;
		i32 b1 = (a8 * multiplyConstants[0]) >> fixMultiplyShift;
		i32 b2 = (a7 * multiplyConstants[1]) >> fixMultiplyShift;
		i32 b3 = (a5 * multiplyConstants[1]) >> fixMultiplyShift;
		i32 b4 = (a7 * multiplyConstants[2]) >> fixMultiplyShift;
		i32 b5 = (a5 * multiplyConstants[2]) >> fixMultiplyShift;
		i32 b6 = b5 - b2;
		i32 b7 = b3 + b4;

		//Stage 3: Post-merge
		i32 bc = a2 - b0;
		i32 c0 = a0 + b0;
		i32 c1 = a1 + bc;
		i32 c2 = a1 - bc;
		i32 c3 = a0 - b0;
		i32 c4 = b1 + b6;
		i32 c5 = a9 - b7;
		i32 c6 = b6;
		i32 c7 = b7 - b1;

		//Stage 4: Transposed output
		buffer[8 * 0 + i] = c0 + c7;
		buffer[8 * 1 + i] = c1 + c6;
		buffer[8 * 2 + i] = c2 + c5;
		buffer[8 * 3 + i] = c3 + c4;
		buffer[8 * 4 + i] = c3 - c4;
		buffer[8 * 5 + i] = c2 - c5;
		buffer[8 * 6 + i] = c1 - c6;
		buffer[8 * 7 + i] = c0 - c7;

	}

	i32 tmp[8];

	//Second pass
	for (u32 i = 0; i < sizeY; i++) {

		u32 k = i * 8;

		//Stage 1: Pre-multiplication stage
		i32 a0 = buffer[k + 0] + buffer[k + 4];
		i32 a1 = buffer[k + 0] - buffer[k + 4];
		i32 a2 = buffer[k + 2] - buffer[k + 6];
		i32 a3 = buffer[k + 2] + buffer[k + 6];
		i32 a4 = buffer[k + 1] + buffer[k + 7];
		i32 a5 = buffer[k + 1] - buffer[k + 7];
		i32 a6 = buffer[k + 5] - buffer[k + 3];
		i32 a7 = buffer[k + 5] + buffer[k + 3];
		i32 a8 = a6 - a4;
		i32 a9 = a6 + a4;

		//Stage 2: Multiplication
		i32 b0 = (a3 * multiplyConstants[0]) >> fixMultiplyShift;
		i32 b1 = (a8 * multiplyConstants[0]) >> fixMultiplyShift;
		i32 b2 = (a7 * multiplyConstants[1]) >> fixMultiplyShift;
		i32 b3 = (a5 * multiplyConstants[1]) >> fixMultiplyShift;
		i32 b4 = (a7 * multiplyConstants[2]) >> fixMultiplyShift;
		i32 b5 = (a5 * multiplyConstants[2]) >> fixMultiplyShift;
		i32 b6 = b5 - b2;
		i32 b7 = b3 + b4;

		//Stage 3: Post-merge
		i32 bc = a2 - b0;
		i32 c0 = a0 + b0;
		i32 c1 = a1 + bc;
		i32 c2 = a1 - bc;
		i32 c3 = a0 - b0;
		i32 c4 = b1 + b6;
		i32 c5 = a9 - b7;
		i32 c6 = b6;
		i32 c7 = b7 - b1;

		//Stage 4: Final output, block-to-image transform
		tmp[0] = c0 + c7;
		tmp[1] = c1 + c6;
		tmp[2] = c2 + c5;
		tmp[3] = c3 + c4;
		tmp[4] = c3 - c4;
		tmp[5] = c2 - c5;
		tmp[6] = c1 - c6;
		tmp[7] = c0 - c7;

		for (u32 j = 0; j < sizeX; j++) {
			outData[outStride * i + j] = static_cast<i16>((tmp[j] >> (fixScaleShift - fixTransformShift)) + (128 << fixTransformShift));
		}

	}

}



void JPEGDecoder::applyIDCT(JPEG::ScanComponent& component, SizeT imageBase) {

	const i32* inData = component.block;            //Aligned to 32 bytes
	i16* outData = &component.frameComponent.imageData[imageBase];

#ifdef ARC_VECTORIZE_X86_AVX2

	const __m256i* inVec = reinterpret_cast<const __m256i*>(inData);

	__m256i bufVec[8];

	__m256i m0 = _mm256_set1_epi32(multiplyConstants[0]);
	__m256i m1 = _mm256_set1_epi32(multiplyConstants[1]);
	__m256i m2 = _mm256_set1_epi32(multiplyConstants[2]);
	__m256i m3 = _mm256_set1_epi32(128 << fixTransformShift);
	__m256i m4 = _mm256_setr_epi64x(1LL << 63, 1LL << 63, 0, 0);
	__m256i m5 = _mm256_setr_epi64x(0, 0, 1LL << 63, 1LL << 63);

	__int64* outVec[8];

	for (u32 i = 0; i < 8; i++) {
		outVec[i] = reinterpret_cast<__int64*>(outData + component.frameComponent.width * i - (i & 1) * 8);
	}

	/*
	 *  This algorithm is based on the scalar version, unrolled 8 times
	 */
	{
		__m256i in0 = _mm256_load_si256(inVec + 0);
		__m256i in1 = _mm256_load_si256(inVec + 1);
		__m256i in2 = _mm256_load_si256(inVec + 2);
		__m256i in3 = _mm256_load_si256(inVec + 3);
		__m256i in4 = _mm256_load_si256(inVec + 4);
		__m256i in5 = _mm256_load_si256(inVec + 5);
		__m256i in6 = _mm256_load_si256(inVec + 6);
		__m256i in7 = _mm256_load_si256(inVec + 7);

		__m256i a0 = _mm256_add_epi32(in0, in4);
		__m256i a1 = _mm256_sub_epi32(in0, in4);
		__m256i a2 = _mm256_sub_epi32(in2, in6);
		__m256i a3 = _mm256_add_epi32(in2, in6);
		__m256i a4 = _mm256_add_epi32(in1, in7);
		__m256i a5 = _mm256_sub_epi32(in1, in7);
		__m256i a6 = _mm256_sub_epi32(in5, in3);
		__m256i a7 = _mm256_add_epi32(in5, in3);
		__m256i a8 = _mm256_sub_epi32(a6, a4);
		__m256i a9 = _mm256_add_epi32(a6, a4);

		__m256i b0 = _mm256_srai_epi32(_mm256_mullo_epi32(a3, m0), fixMultiplyShift);
		__m256i b1 = _mm256_srai_epi32(_mm256_mullo_epi32(a8, m0), fixMultiplyShift);
		__m256i b2 = _mm256_srai_epi32(_mm256_mullo_epi32(a7, m1), fixMultiplyShift);
		__m256i b3 = _mm256_srai_epi32(_mm256_mullo_epi32(a5, m1), fixMultiplyShift);
		__m256i b4 = _mm256_srai_epi32(_mm256_mullo_epi32(a7, m2), fixMultiplyShift);
		__m256i b5 = _mm256_srai_epi32(_mm256_mullo_epi32(a5, m2), fixMultiplyShift);
		__m256i b6 = _mm256_sub_epi32(b5, b2);
		__m256i b7 = _mm256_add_epi32(b3, b4);

		__m256i bc = _mm256_sub_epi32(a2, b0);
		__m256i c0 = _mm256_add_epi32(a0, b0);
		__m256i c1 = _mm256_add_epi32(a1, bc);
		__m256i c2 = _mm256_sub_epi32(a1, bc);
		__m256i c3 = _mm256_sub_epi32(a0, b0);
		__m256i c4 = _mm256_add_epi32(b1, b6);
		__m256i c5 = _mm256_sub_epi32(a9, b7);
		__m256i c6 = b6;
		__m256i c7 = _mm256_sub_epi32(b7, b1);

		__m256i d0 = _mm256_add_epi32(c0, c7);
		__m256i d1 = _mm256_add_epi32(c1, c6);
		__m256i d2 = _mm256_add_epi32(c2, c5);
		__m256i d3 = _mm256_add_epi32(c3, c4);
		__m256i d4 = _mm256_sub_epi32(c3, c4);
		__m256i d5 = _mm256_sub_epi32(c2, c5);
		__m256i d6 = _mm256_sub_epi32(c1, c6);
		__m256i d7 = _mm256_sub_epi32(c0, c7);

		//8x8 flat transpose
		__m256i e0 = _mm256_unpacklo_epi32(d0, d1);     //a0, b0, a1, b1, a4, b4, a5, b5
		__m256i e1 = _mm256_unpacklo_epi32(d2, d3);     //c0, d0, c1, d1, c4, d4, c5, d5
		__m256i e2 = _mm256_unpacklo_epi32(d4, d5);     //e0, f0, e1, f1, e4, f4, e5, f5
		__m256i e3 = _mm256_unpacklo_epi32(d6, d7);     //g0, h0, g1, h1, g4, h4, g5, h5
		__m256i e4 = _mm256_unpackhi_epi32(d0, d1);     //a2, b2, a3, b3, a6, b6, a7, b7
		__m256i e5 = _mm256_unpackhi_epi32(d2, d3);     //c2, d2, c3, d3, c6, d6, c7, d7
		__m256i e6 = _mm256_unpackhi_epi32(d4, d5);     //e2, f2, e3, f3, e6, f6, e7, f7
		__m256i e7 = _mm256_unpackhi_epi32(d6, d7);     //g2, h2, g3, h3, g6, h6, g7, h7

		__m256i f0 = _mm256_shuffle_epi32(e1, 0x4E);    //c1, d1, c0, d0, c5, d5, c4, d4
		__m256i f1 = _mm256_shuffle_epi32(e3, 0x4E);    //g1, h1, g0, h0, g5, h5, g4, h4
		__m256i f2 = _mm256_shuffle_epi32(e5, 0x4E);    //c3, d3, c2, d2, c7, d7, c6, d6
		__m256i f3 = _mm256_shuffle_epi32(e7, 0x4E);    //g3, h3, g2, h2, g7, h7, g6, h6

		__m256i g0 = _mm256_blend_epi32(e0, f0, 0xCC);  //a0, b0, c0, d0, a4, b4, c4, d4
		__m256i g1 = _mm256_blend_epi32(e2, f1, 0xCC);  //e0, f0, g0, h0, e4, f4, g4, h4
		__m256i g2 = _mm256_blend_epi32(e4, f2, 0xCC);  //a2, b2, c2, d2, a6, b6, c6, d6
		__m256i g3 = _mm256_blend_epi32(e6, f3, 0xCC);  //e2, f2, g2, h2, e6, f6, g6, h6
		__m256i g4 = _mm256_blend_epi32(e0, f0, 0x33);  //c1, d1, a1, b1, c5, d5, a5, b5
		__m256i g5 = _mm256_blend_epi32(e2, f1, 0x33);  //g1, h1, e1, f1, g5, h5, e5, f5
		__m256i g6 = _mm256_blend_epi32(e4, f2, 0x33);  //c3, d3, a3, b3, c7, d7, a7, b7
		__m256i g7 = _mm256_blend_epi32(e6, f3, 0x33);  //g3, h3, e3, f3, g7, h7, e7, f7

		__m256i h0 = _mm256_shuffle_epi32(g4, 0x4E);    //a1, b1, c1, d1, a5, b5, c5, d5
		__m256i h1 = _mm256_shuffle_epi32(g5, 0x4E);    //e1, f1, g1, h1, e5, f5, g5, h5
		__m256i h2 = _mm256_shuffle_epi32(g6, 0x4E);    //a3, b3, c3, d3, a7, b7, c7, d7
		__m256i h3 = _mm256_shuffle_epi32(g7, 0x4E);    //e3, f3, g3, h3, e7, f7, g7, h7

		_mm256_store_si256(bufVec + 0, _mm256_permute2x128_si256(g0, g1, 0x20));
		_mm256_store_si256(bufVec + 1, _mm256_permute2x128_si256(h0, h1, 0x20));
		_mm256_store_si256(bufVec + 2, _mm256_permute2x128_si256(g2, g3, 0x20));
		_mm256_store_si256(bufVec + 3, _mm256_permute2x128_si256(h2, h3, 0x20));
		_mm256_store_si256(bufVec + 4, _mm256_permute2x128_si256(g0, g1, 0x31));
		_mm256_store_si256(bufVec + 5, _mm256_permute2x128_si256(h0, h1, 0x31));
		_mm256_store_si256(bufVec + 6, _mm256_permute2x128_si256(g2, g3, 0x31));
		_mm256_store_si256(bufVec + 7, _mm256_permute2x128_si256(h2, h3, 0x31));
	}

	{
		__m256i in0 = _mm256_load_si256(bufVec + 0);
		__m256i in1 = _mm256_load_si256(bufVec + 1);
		__m256i in2 = _mm256_load_si256(bufVec + 2);
		__m256i in3 = _mm256_load_si256(bufVec + 3);
		__m256i in4 = _mm256_load_si256(bufVec + 4);
		__m256i in5 = _mm256_load_si256(bufVec + 5);
		__m256i in6 = _mm256_load_si256(bufVec + 6);
		__m256i in7 = _mm256_load_si256(bufVec + 7);

		__m256i a0 = _mm256_add_epi32(in0, in4);
		__m256i a1 = _mm256_sub_epi32(in0, in4);
		__m256i a2 = _mm256_sub_epi32(in2, in6);
		__m256i a3 = _mm256_add_epi32(in2, in6);
		__m256i a4 = _mm256_add_epi32(in1, in7);
		__m256i a5 = _mm256_sub_epi32(in1, in7);
		__m256i a6 = _mm256_sub_epi32(in5, in3);
		__m256i a7 = _mm256_add_epi32(in5, in3);
		__m256i a8 = _mm256_sub_epi32(a6, a4);
		__m256i a9 = _mm256_add_epi32(a6, a4);

		__m256i b0 = _mm256_srai_epi32(_mm256_mullo_epi32(a3, m0), fixMultiplyShift);
		__m256i b1 = _mm256_srai_epi32(_mm256_mullo_epi32(a8, m0), fixMultiplyShift);
		__m256i b2 = _mm256_srai_epi32(_mm256_mullo_epi32(a7, m1), fixMultiplyShift);
		__m256i b3 = _mm256_srai_epi32(_mm256_mullo_epi32(a5, m1), fixMultiplyShift);
		__m256i b4 = _mm256_srai_epi32(_mm256_mullo_epi32(a7, m2), fixMultiplyShift);
		__m256i b5 = _mm256_srai_epi32(_mm256_mullo_epi32(a5, m2), fixMultiplyShift);
		__m256i b6 = _mm256_sub_epi32(b5, b2);
		__m256i b7 = _mm256_add_epi32(b3, b4);

		__m256i bc = _mm256_sub_epi32(a2, b0);
		__m256i c0 = _mm256_add_epi32(a0, b0);
		__m256i c1 = _mm256_add_epi32(a1, bc);
		__m256i c2 = _mm256_sub_epi32(a1, bc);
		__m256i c3 = _mm256_sub_epi32(a0, b0);
		__m256i c4 = _mm256_add_epi32(b1, b6);
		__m256i c5 = _mm256_sub_epi32(a9, b7);
		__m256i c6 = b6;
		__m256i c7 = _mm256_sub_epi32(b7, b1);

		__m256i d0 = _mm256_add_epi32(_mm256_srai_epi32(_mm256_add_epi32(c0, c7), (fixScaleShift - fixTransformShift)), m3);
		__m256i d1 = _mm256_add_epi32(_mm256_srai_epi32(_mm256_add_epi32(c1, c6), (fixScaleShift - fixTransformShift)), m3);
		__m256i d2 = _mm256_add_epi32(_mm256_srai_epi32(_mm256_add_epi32(c2, c5), (fixScaleShift - fixTransformShift)), m3);
		__m256i d3 = _mm256_add_epi32(_mm256_srai_epi32(_mm256_add_epi32(c3, c4), (fixScaleShift - fixTransformShift)), m3);
		__m256i d4 = _mm256_add_epi32(_mm256_srai_epi32(_mm256_sub_epi32(c3, c4), (fixScaleShift - fixTransformShift)), m3);
		__m256i d5 = _mm256_add_epi32(_mm256_srai_epi32(_mm256_sub_epi32(c2, c5), (fixScaleShift - fixTransformShift)), m3);
		__m256i d6 = _mm256_add_epi32(_mm256_srai_epi32(_mm256_sub_epi32(c1, c6), (fixScaleShift - fixTransformShift)), m3);
		__m256i d7 = _mm256_add_epi32(_mm256_srai_epi32(_mm256_sub_epi32(c0, c7), (fixScaleShift - fixTransformShift)), m3);

		__m256i e0 = _mm256_permute4x64_epi64(_mm256_packs_epi32(d0, d1), 0xD8);
		__m256i e1 = _mm256_permute4x64_epi64(_mm256_packs_epi32(d2, d3), 0xD8);
		__m256i e2 = _mm256_permute4x64_epi64(_mm256_packs_epi32(d4, d5), 0xD8);
		__m256i e3 = _mm256_permute4x64_epi64(_mm256_packs_epi32(d6, d7), 0xD8);

		_mm256_maskstore_epi64(outVec[0], m4, e0);
		_mm256_maskstore_epi64(outVec[1], m5, e0);
		_mm256_maskstore_epi64(outVec[2], m4, e1);
		_mm256_maskstore_epi64(outVec[3], m5, e1);
		_mm256_maskstore_epi64(outVec[4], m4, e2);
		_mm256_maskstore_epi64(outVec[5], m5, e2);
		_mm256_maskstore_epi64(outVec[6], m4, e3);
		_mm256_maskstore_epi64(outVec[7], m5, e3);
	}

#elif defined(ARC_VECTORIZE_X86_SSE4_1)     //pmulld requires SSE4.1, possible improvement through optimized shifts + adds?

	const __m128i* inVec = reinterpret_cast<const __m128i*>(inData);

	__m128i bufVec[16];

	__m128i m0 = _mm_set1_epi32(multiplyConstants[0]);
	__m128i m1 = _mm_set1_epi32(multiplyConstants[1]);
	__m128i m2 = _mm_set1_epi32(multiplyConstants[2]);
	__m128i m3 = _mm_set1_epi32(128 << fixTransformShift);

	__m128i* outVec[8];

	for (u32 i = 0; i < 8; i++) {
		outVec[i] = reinterpret_cast<__m128i*>(outData + component.width * i);
	}

	/*
	 *  This algorithm is based on the scalar version, unrolled 4 times
	 *  Taking the same shortcut as in the scalar version is slower since all
	 *  input vectors need to take it, which happens only very rarely.
	 */
	for (u32 i = 0; i < 2; i++) {

		__m128i in0 = _mm_load_si128(inVec + i +  0);
		__m128i in1 = _mm_load_si128(inVec + i +  2);
		__m128i in2 = _mm_load_si128(inVec + i +  4);
		__m128i in3 = _mm_load_si128(inVec + i +  6);
		__m128i in4 = _mm_load_si128(inVec + i +  8);
		__m128i in5 = _mm_load_si128(inVec + i + 10);
		__m128i in6 = _mm_load_si128(inVec + i + 12);
		__m128i in7 = _mm_load_si128(inVec + i + 14);

		__m128i a0 = _mm_add_epi32(in0, in4);
		__m128i a1 = _mm_sub_epi32(in0, in4);
		__m128i a2 = _mm_sub_epi32(in2, in6);
		__m128i a3 = _mm_add_epi32(in2, in6);
		__m128i a4 = _mm_add_epi32(in1, in7);
		__m128i a5 = _mm_sub_epi32(in1, in7);
		__m128i a6 = _mm_sub_epi32(in5, in3);
		__m128i a7 = _mm_add_epi32(in5, in3);
		__m128i a8 = _mm_sub_epi32(a6, a4);
		__m128i a9 = _mm_add_epi32(a6, a4);

		__m128i b0 = _mm_srai_epi32(_mm_mullo_epi32(a3, m0), fixMultiplyShift);
		__m128i b1 = _mm_srai_epi32(_mm_mullo_epi32(a8, m0), fixMultiplyShift);
		__m128i b2 = _mm_srai_epi32(_mm_mullo_epi32(a7, m1), fixMultiplyShift);
		__m128i b3 = _mm_srai_epi32(_mm_mullo_epi32(a5, m1), fixMultiplyShift);
		__m128i b4 = _mm_srai_epi32(_mm_mullo_epi32(a7, m2), fixMultiplyShift);
		__m128i b5 = _mm_srai_epi32(_mm_mullo_epi32(a5, m2), fixMultiplyShift);
		__m128i b6 = _mm_sub_epi32(b5, b2);
		__m128i b7 = _mm_add_epi32(b3, b4);

		__m128i bc = _mm_sub_epi32(a2, b0);
		__m128i c0 = _mm_add_epi32(a0, b0);
		__m128i c1 = _mm_add_epi32(a1, bc);
		__m128i c2 = _mm_sub_epi32(a1, bc);
		__m128i c3 = _mm_sub_epi32(a0, b0);
		__m128i c4 = _mm_add_epi32(b1, b6);
		__m128i c5 = _mm_sub_epi32(a9, b7);
		__m128i c6 = b6;
		__m128i c7 = _mm_sub_epi32(b7, b1);

		__m128i d0 = _mm_add_epi32(c0, c7);
		__m128i d1 = _mm_add_epi32(c1, c6);
		__m128i d2 = _mm_add_epi32(c2, c5);
		__m128i d3 = _mm_add_epi32(c3, c4);
		__m128i d4 = _mm_sub_epi32(c3, c4);
		__m128i d5 = _mm_sub_epi32(c2, c5);
		__m128i d6 = _mm_sub_epi32(c1, c6);
		__m128i d7 = _mm_sub_epi32(c0, c7);

		//Transpose
		__m128i e0 = _mm_unpacklo_epi32(d0, d1);
		__m128i e1 = _mm_unpacklo_epi32(d2, d3);
		__m128i e2 = _mm_unpackhi_epi32(d0, d1);
		__m128i e3 = _mm_unpackhi_epi32(d2, d3);
		__m128i e4 = _mm_unpacklo_epi32(d4, d5);
		__m128i e5 = _mm_unpacklo_epi32(d6, d7);
		__m128i e6 = _mm_unpackhi_epi32(d4, d5);
		__m128i e7 = _mm_unpackhi_epi32(d6, d7);

		_mm_store_si128(bufVec + i * 8 + 0, _mm_shuffle_epi32(_mm_unpacklo_epi32(e0, e1), 0xD8));
		_mm_store_si128(bufVec + i * 8 + 2, _mm_shuffle_epi32(_mm_unpackhi_epi32(e0, e1), 0xD8));
		_mm_store_si128(bufVec + i * 8 + 4, _mm_shuffle_epi32(_mm_unpacklo_epi32(e2, e3), 0xD8));
		_mm_store_si128(bufVec + i * 8 + 6, _mm_shuffle_epi32(_mm_unpackhi_epi32(e2, e3), 0xD8));
		_mm_store_si128(bufVec + i * 8 + 1, _mm_shuffle_epi32(_mm_unpacklo_epi32(e4, e5), 0xD8));
		_mm_store_si128(bufVec + i * 8 + 3, _mm_shuffle_epi32(_mm_unpackhi_epi32(e4, e5), 0xD8));
		_mm_store_si128(bufVec + i * 8 + 5, _mm_shuffle_epi32(_mm_unpacklo_epi32(e6, e7), 0xD8));
		_mm_store_si128(bufVec + i * 8 + 7, _mm_shuffle_epi32(_mm_unpackhi_epi32(e6, e7), 0xD8));

	}

	{

		__m128i inA0 = _mm_load_si128(bufVec + 0 +  0);
		__m128i inA1 = _mm_load_si128(bufVec + 0 +  2);
		__m128i inA2 = _mm_load_si128(bufVec + 0 +  4);
		__m128i inA3 = _mm_load_si128(bufVec + 0 +  6);
		__m128i inA4 = _mm_load_si128(bufVec + 0 +  8);
		__m128i inA5 = _mm_load_si128(bufVec + 0 + 10);
		__m128i inA6 = _mm_load_si128(bufVec + 0 + 12);
		__m128i inA7 = _mm_load_si128(bufVec + 0 + 14);
		__m128i inB0 = _mm_load_si128(bufVec + 1 +  0);
		__m128i inB1 = _mm_load_si128(bufVec + 1 +  2);
		__m128i inB2 = _mm_load_si128(bufVec + 1 +  4);
		__m128i inB3 = _mm_load_si128(bufVec + 1 +  6);
		__m128i inB4 = _mm_load_si128(bufVec + 1 +  8);
		__m128i inB5 = _mm_load_si128(bufVec + 1 + 10);
		__m128i inB6 = _mm_load_si128(bufVec + 1 + 12);
		__m128i inB7 = _mm_load_si128(bufVec + 1 + 14);

		__m128i aA0 = _mm_add_epi32(inA0, inA4);
		__m128i aA1 = _mm_sub_epi32(inA0, inA4);
		__m128i aA2 = _mm_sub_epi32(inA2, inA6);
		__m128i aA3 = _mm_add_epi32(inA2, inA6);
		__m128i aA4 = _mm_add_epi32(inA1, inA7);
		__m128i aA5 = _mm_sub_epi32(inA1, inA7);
		__m128i aA6 = _mm_sub_epi32(inA5, inA3);
		__m128i aA7 = _mm_add_epi32(inA5, inA3);
		__m128i aA8 = _mm_sub_epi32(aA6, aA4);
		__m128i aA9 = _mm_add_epi32(aA6, aA4);
		__m128i aB0 = _mm_add_epi32(inB0, inB4);
		__m128i aB1 = _mm_sub_epi32(inB0, inB4);
		__m128i aB2 = _mm_sub_epi32(inB2, inB6);
		__m128i aB3 = _mm_add_epi32(inB2, inB6);
		__m128i aB4 = _mm_add_epi32(inB1, inB7);
		__m128i aB5 = _mm_sub_epi32(inB1, inB7);
		__m128i aB6 = _mm_sub_epi32(inB5, inB3);
		__m128i aB7 = _mm_add_epi32(inB5, inB3);
		__m128i aB8 = _mm_sub_epi32(aB6, aB4);
		__m128i aB9 = _mm_add_epi32(aB6, aB4);

		__m128i bA0 = _mm_srai_epi32(_mm_mullo_epi32(aA3, m0), fixMultiplyShift);
		__m128i bA1 = _mm_srai_epi32(_mm_mullo_epi32(aA8, m0), fixMultiplyShift);
		__m128i bA2 = _mm_srai_epi32(_mm_mullo_epi32(aA7, m1), fixMultiplyShift);
		__m128i bA3 = _mm_srai_epi32(_mm_mullo_epi32(aA5, m1), fixMultiplyShift);
		__m128i bA4 = _mm_srai_epi32(_mm_mullo_epi32(aA7, m2), fixMultiplyShift);
		__m128i bA5 = _mm_srai_epi32(_mm_mullo_epi32(aA5, m2), fixMultiplyShift);
		__m128i bA6 = _mm_sub_epi32(bA5, bA2);
		__m128i bA7 = _mm_add_epi32(bA3, bA4);
		__m128i bB0 = _mm_srai_epi32(_mm_mullo_epi32(aB3, m0), fixMultiplyShift);
		__m128i bB1 = _mm_srai_epi32(_mm_mullo_epi32(aB8, m0), fixMultiplyShift);
		__m128i bB2 = _mm_srai_epi32(_mm_mullo_epi32(aB7, m1), fixMultiplyShift);
		__m128i bB3 = _mm_srai_epi32(_mm_mullo_epi32(aB5, m1), fixMultiplyShift);
		__m128i bB4 = _mm_srai_epi32(_mm_mullo_epi32(aB7, m2), fixMultiplyShift);
		__m128i bB5 = _mm_srai_epi32(_mm_mullo_epi32(aB5, m2), fixMultiplyShift);
		__m128i bB6 = _mm_sub_epi32(bB5, bB2);
		__m128i bB7 = _mm_add_epi32(bB3, bB4);

		__m128i bAc = _mm_sub_epi32(aA2, bA0);
		__m128i cA0 = _mm_add_epi32(aA0, bA0);
		__m128i cA1 = _mm_add_epi32(aA1, bAc);
		__m128i cA2 = _mm_sub_epi32(aA1, bAc);
		__m128i cA3 = _mm_sub_epi32(aA0, bA0);
		__m128i cA4 = _mm_add_epi32(bA1, bA6);
		__m128i cA5 = _mm_sub_epi32(aA9, bA7);
		__m128i cA6 = bA6;
		__m128i cA7 = _mm_sub_epi32(bA7, bA1);
		__m128i bBc = _mm_sub_epi32(aB2, bB0);
		__m128i cB0 = _mm_add_epi32(aB0, bB0);
		__m128i cB1 = _mm_add_epi32(aB1, bBc);
		__m128i cB2 = _mm_sub_epi32(aB1, bBc);
		__m128i cB3 = _mm_sub_epi32(aB0, bB0);
		__m128i cB4 = _mm_add_epi32(bB1, bB6);
		__m128i cB5 = _mm_sub_epi32(aB9, bB7);
		__m128i cB6 = bB6;
		__m128i cB7 = _mm_sub_epi32(bB7, bB1);

		__m128i dA0 = _mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(cA0, cA7), (fixScaleShift - fixTransformShift)), m3);
		__m128i dA1 = _mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(cA1, cA6), (fixScaleShift - fixTransformShift)), m3);
		__m128i dA2 = _mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(cA2, cA5), (fixScaleShift - fixTransformShift)), m3);
		__m128i dA3 = _mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(cA3, cA4), (fixScaleShift - fixTransformShift)), m3);
		__m128i dA4 = _mm_add_epi32(_mm_srai_epi32(_mm_sub_epi32(cA3, cA4), (fixScaleShift - fixTransformShift)), m3);
		__m128i dA5 = _mm_add_epi32(_mm_srai_epi32(_mm_sub_epi32(cA2, cA5), (fixScaleShift - fixTransformShift)), m3);
		__m128i dA6 = _mm_add_epi32(_mm_srai_epi32(_mm_sub_epi32(cA1, cA6), (fixScaleShift - fixTransformShift)), m3);
		__m128i dA7 = _mm_add_epi32(_mm_srai_epi32(_mm_sub_epi32(cA0, cA7), (fixScaleShift - fixTransformShift)), m3);
		__m128i dB0 = _mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(cB0, cB7), (fixScaleShift - fixTransformShift)), m3);
		__m128i dB1 = _mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(cB1, cB6), (fixScaleShift - fixTransformShift)), m3);
		__m128i dB2 = _mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(cB2, cB5), (fixScaleShift - fixTransformShift)), m3);
		__m128i dB3 = _mm_add_epi32(_mm_srai_epi32(_mm_add_epi32(cB3, cB4), (fixScaleShift - fixTransformShift)), m3);
		__m128i dB4 = _mm_add_epi32(_mm_srai_epi32(_mm_sub_epi32(cB3, cB4), (fixScaleShift - fixTransformShift)), m3);
		__m128i dB5 = _mm_add_epi32(_mm_srai_epi32(_mm_sub_epi32(cB2, cB5), (fixScaleShift - fixTransformShift)), m3);
		__m128i dB6 = _mm_add_epi32(_mm_srai_epi32(_mm_sub_epi32(cB1, cB6), (fixScaleShift - fixTransformShift)), m3);
		__m128i dB7 = _mm_add_epi32(_mm_srai_epi32(_mm_sub_epi32(cB0, cB7), (fixScaleShift - fixTransformShift)), m3);

		//Pack
		_mm_storeu_si128(outVec[0], _mm_packs_epi32(dA0, dB0));
		_mm_storeu_si128(outVec[1], _mm_packs_epi32(dA1, dB1));
		_mm_storeu_si128(outVec[2], _mm_packs_epi32(dA2, dB2));
		_mm_storeu_si128(outVec[3], _mm_packs_epi32(dA3, dB3));
		_mm_storeu_si128(outVec[4], _mm_packs_epi32(dA4, dB4));
		_mm_storeu_si128(outVec[5], _mm_packs_epi32(dA5, dB5));
		_mm_storeu_si128(outVec[6], _mm_packs_epi32(dA6, dB6));
		_mm_storeu_si128(outVec[7], _mm_packs_epi32(dA7, dB7));

	}

#else

	scalarIDCT<true>(inData, outData, component.frameComponent.width, 8, 8);

#endif

}



void JPEGDecoder::applyPartialIDCT(JPEG::ScanComponent& component, SizeT imageBase, u32 width, u32 height) {
	scalarIDCT<false>(component.block, &component.frameComponent.imageData[imageBase], component.frameComponent.width, width, height);
}



void JPEGDecoder::blendAndUpsample() {

	bool lossless = frame.type == FrameType::Lossless;

	switch (scan.scanComponents.size()) {

		case 1:
			lossless ? blendMonochromeTransformless() : blendMonochrome();
			break;

		case 2:
			break;

		case 3:
			lossless ? blendAndUpsampleYCbCrTransformless() : blendAndUpsampleYCbCr();
			break;

		case 4:
			break;

		default:
			std::unreachable();

	}

}



template<u32 FixShift>
static RawImage blendMonochromeCore(Scan& scan) {

	const JPEG::ScanComponent& component = scan.scanComponents[0];
	const JPEG::FrameComponent& frameComponent = component.frameComponent;

	Image<Pixel::Grayscale8> target(frameComponent.width, frameComponent.height);

	SizeT offset = 0;

#ifdef ARC_VECTORIZE_X86_SSE4_1

	SizeT totalPixels = target.pixelCount();
	SizeT vectorSize = totalPixels / 16;
	SizeT scalarSize = totalPixels % 16;

	__m128i* targetVecData = reinterpret_cast<__m128i*>(target.getImageBuffer().data());
	const __m128i* vecData = reinterpret_cast<const __m128i*>(frameComponent.imageData.data());

	__m128i bias = _mm_set1_epi16(colorBias<FixShift>);

	for (SizeT i = 0; i < vectorSize; i++) {

		__m128i v0 = _mm_load_si128(vecData + 0);
		__m128i v1 = _mm_load_si128(vecData + 1);

		v0 = _mm_srai_epi16(_mm_add_epi16(v0, bias), FixShift);
		v1 = _mm_srai_epi16(_mm_add_epi16(v1, bias), FixShift);

		_mm_storeu_si128(targetVecData++, _mm_packus_epi16(v0, v1));

		vecData += 2;

	}

	const i16* imgData = reinterpret_cast<const i16*>(vecData);
	u8* targetSclData = Bits::toByteArray(targetVecData);

	for (SizeT i = 0; i < scalarSize; i++) {

		*targetSclData = (*imgData + colorBias<FixShift>) >> FixShift;

		targetSclData++;
		imgData++;

	}

#else

	for (u32 i = 0; i < target.getHeight(); i++) {

		for (u32 j = 0; j < target.getWidth(); j++) {

			target.setPixel(j, i, PixelGrayscale8(Math::clamp((component.frameComponent.imageData[offset++] + colorBias<FixShift>) >> FixShift, 0, 255)));

		}

	}

#endif

	return target.makeRaw();

}



template<u32 FixShift>
static RawImage blendAndUpsampleYCbCrCore(Scan& scan) {

	enum class Subsampling {
		None,
		Horizontal,
		Vertical,
		Both
	};

	auto exec = [&]<Subsampling S>() -> RawImage {

		const JPEG::ScanComponent& component = scan.scanComponents[0];
		const JPEG::FrameComponent& frameComponent = component.frameComponent;

		Image<Pixel::RGB8> target(frameComponent.width, frameComponent.height);

		const i16* imgData[3] = {scan.scanComponents[0].frameComponent.imageData.data(),
								 scan.scanComponents[1].frameComponent.imageData.data(),
								 scan.scanComponents[2].frameComponent.imageData.data()};

	#ifdef ARC_VECTORIZE_X86_SSE4_1

		if constexpr (S != Subsampling::None) {
			throw UnsupportedOperationException("Subsampling not yet vectorized");
		}

		SizeT totalPixels = target.pixelCount();
		SizeT vectorSize = totalPixels / 16;
		SizeT scalarSize = totalPixels % 16;

		i32 yShift = 15 - ycbcrShift;
		i32 rgbShift = FixShift + ycbcrShift - 15;

		__m128i rcr = _mm_set1_epi16(ycbcrFactors[0]);
		__m128i gcb = _mm_set1_epi16(ycbcrFactors[1]);
		__m128i gcr = _mm_set1_epi16(ycbcrFactors[2]);
		__m128i bcb = _mm_set1_epi16(ycbcrFactors[3]);
		__m128i csb = _mm_set1_epi16(128 << FixShift);
		__m128i bias = _mm_set1_epi16(i16(1 << (rgbShift - 1)));

		__m128i shuf0 = _mm_setr_epi32(0x0D070605, 0x01000F0E, 0x08040302, 0x0C0B0A09);
		__m128i shuf1 = _mm_setr_epi32(0x06050403, 0x0D0C0B07, 0x01000F0E, 0x0A090802);
		__m128i shuf2 = _mm_setr_epi32(0x010B0600, 0x08020C07, 0x0E09030D, 0x050F0A04);
		__m128i shuf3 = _mm_setr_epi32(0x01060300, 0x05020704, 0x0B080D0A, 0x0F0C090E);
		__m128i shuf4 = _mm_setr_epi32(0x0B05000A, 0x020C0601, 0x08030D07, 0x0F09040E);

		__m128i* targetVecData = reinterpret_cast<__m128i*>(target.getImageBuffer().data());
		const __m128i* vecData[3] = { reinterpret_cast<const __m128i*>(imgData[0]),
									  reinterpret_cast<const __m128i*>(imgData[1]),
									  reinterpret_cast<const __m128i*>(imgData[2]) };

		for (SizeT i = 0; i < vectorSize; i++) {

			__m128i l0 = _mm_srai_epi16(_mm_load_si128(vecData[0] + 0), yShift);
			__m128i l1 = _mm_srai_epi16(_mm_load_si128(vecData[0] + 1), yShift);
			__m128i l2 = _mm_sub_epi16(_mm_load_si128(vecData[1] + 0), csb);
			__m128i l3 = _mm_sub_epi16(_mm_load_si128(vecData[1] + 1), csb);
			__m128i l4 = _mm_sub_epi16(_mm_load_si128(vecData[2] + 0), csb);
			__m128i l5 = _mm_sub_epi16(_mm_load_si128(vecData[2] + 1), csb);

			__m128i rx0 = _mm_mulhrs_epi16(l4, rcr);
			__m128i gx0 = _mm_mulhrs_epi16(l2, gcb);
			__m128i gy0 = _mm_mulhrs_epi16(l4, gcr);
			__m128i bx0 = _mm_mulhrs_epi16(l2, bcb);
			__m128i rx1 = _mm_mulhrs_epi16(l5, rcr);
			__m128i gx1 = _mm_mulhrs_epi16(l3, gcb);
			__m128i gy1 = _mm_mulhrs_epi16(l5, gcr);
			__m128i bx1 = _mm_mulhrs_epi16(l3, bcb);

			__m128i r0 = _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(l0, rx0), bias), rgbShift);
			__m128i g0 = _mm_srai_epi16(_mm_add_epi16(_mm_sub_epi16(_mm_sub_epi16(l0, gx0), gy0), bias), rgbShift);
			__m128i b0 = _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(l0, bx0), bias), rgbShift);
			__m128i r1 = _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(l1, rx1), bias), rgbShift);
			__m128i g1 = _mm_srai_epi16(_mm_add_epi16(_mm_sub_epi16(_mm_sub_epi16(l1, gx1), gy1), bias), rgbShift);
			__m128i b1 = _mm_srai_epi16(_mm_add_epi16(_mm_add_epi16(l1, bx1), bias), rgbShift);

			__m128i m6 = _mm_packus_epi16(g0, b0);      //g0, g1, g2, g3, g4, [g5, g6, g7], b0, b1, b2, b3, b4, [b5, b6, b7]
			__m128i m7 = _mm_packus_epi16(r0, b1);      //[r0, r1, r2, r3, r4, r5], r6, r7, b8, b9, [bA, bB, bC, bD, bE, bF]
			__m128i m8 = _mm_packus_epi16(r1, g1);      //[r8, r9, rA], rB, rC, rD, rE, rF, [g8, g9, gA], gB, gC, gD, gE, gF

			__m128i n0 = _mm_shuffle_epi8(m6, shuf0);   //[g5, g6, g7, b5, b6, b7], g0, g1, g2, g3, g4, b0, b1, b2, b3, b4
			__m128i n1 = _mm_shuffle_epi8(m8, shuf1);   //rB, rC, rD, rE, rF, gB, gC, gD, gE, gF, [r8, r9, rA, g8, g9, gA]

			__m128i n2 = _mm_blend_epi16(n0, m7, 0x07);                             //r0, r1, r2, r3, r4, r5, g0, g1, g2, g3, g4, b0, b1, b2, b3, b4
			__m128i n3 = _mm_blend_epi16(_mm_blend_epi16(m7, n0, 0x07), n1, 0xE0);  //g5, g6, g7, b5, b6, b7, r6, r7, b8, b9, r8, r9, rA, g8, g9, gA
			__m128i n4 = _mm_blend_epi16(n1, m7, 0xE0);                             //rB, rC, rD, rE, rF, gB, gC, gD, gE, gF, bA, bB, bC, bD, bE, bF

			__m128i c0 = _mm_shuffle_epi8(n2, shuf2);   //r0, g0, b0, r1, g1, b1, r2, g2, b2, r3, g3, b3, r4, g4, b4, r5
			__m128i c1 = _mm_shuffle_epi8(n3, shuf3);   //g5, b5, r6, g6, b6, r7, g7, b7, r8, g8, b8, r9, g9, b9, rA, gA
			__m128i c2 = _mm_shuffle_epi8(n4, shuf4);   //bA, rB, gB, bB, rC, gC, bC, rD, gD, bD, rE, gE, bE, rF, gF, bF

			_mm_storeu_si128(targetVecData + 0, c0);
			_mm_storeu_si128(targetVecData + 1, c1);
			_mm_storeu_si128(targetVecData + 2, c2);

			targetVecData += 3;

			for (u32 j = 0; j < 3; j++) {
				vecData[j] += 2;
			}

		}

		u8* targetSclData = Bits::toByteArray(targetVecData);

		for (u32 i = 0; i < 3; i++) {
			imgData[i] = reinterpret_cast<const i16*>(vecData[i]);
		}

		for (SizeT i = 0; i < scalarSize; i++) {

			//YCbCr to RGB
			i32 y  = *imgData[0];
			i32 cb = *imgData[1] - (128 << FixShift);
			i32 cr = *imgData[2] - (128 << FixShift);

			i32 r = y + ((cr * ycbcrFactors[0]) >> ycbcrShift);
			i32 g = y - ((cb * ycbcrFactors[1]) >> ycbcrShift) - ((cr * ycbcrFactors[2]) >> ycbcrShift);
			i32 b = y + ((cb * ycbcrFactors[3]) >> ycbcrShift);

			u8 rb = Math::clamp((r + colorBias<FixShift>) >> FixShift, 0, 255);
			u8 gb = Math::clamp((g + colorBias<FixShift>) >> FixShift, 0, 255);
			u8 bb = Math::clamp((b + colorBias<FixShift>) >> FixShift, 0, 255);

			targetSclData[0] = rb;
			targetSclData[1] = gb;
			targetSclData[2] = bb;

			targetSclData += 3;

			for (u32 j = 0; j < 3; j++) {
				imgData[j]++;
			}

		}

	#else

		SizeT lumaOffset = 0;
		SizeT chromaBase = 0;
		SizeT chromaOffset = 0;
		SizeT chromaAdvance = S == Subsampling::Horizontal ? target.getWidth() : (target.getWidth() + 1) / 2;

		for (u32 i = 0; i < target.getHeight(); i++) {

			for (u32 j = 0; j < target.getWidth(); j++) {

				if constexpr (S == Subsampling::None) {
					chromaOffset = lumaOffset;
				} else if constexpr (S == Subsampling::Horizontal) {
					chromaOffset = chromaBase + j;
				} else {
					chromaOffset = chromaBase + j / 2;
				}

				i32 y = imgData[0][lumaOffset++];
				i32 cb = imgData[1][chromaOffset] - (128 << FixShift);
				i32 cr = imgData[2][chromaOffset] - (128 << FixShift);

				i32 r = y + ((cr * ycbcrFactors[0]) >> ycbcrShift);
				i32 g = y - ((cb * ycbcrFactors[1]) >> ycbcrShift) - ((cr * ycbcrFactors[2]) >> ycbcrShift);
				i32 b = y + ((cb * ycbcrFactors[3]) >> ycbcrShift);

				target.setPixel(j, i, PixelRGB8(Math::clamp((r + colorBias<FixShift>) >> FixShift, 0, 255), Math::clamp((g + colorBias<FixShift>) >> FixShift, 0, 255), Math::clamp((b + colorBias<FixShift>) >> FixShift, 0, 255)));

			}

			if constexpr (Bool::any(S, Subsampling::Horizontal, Subsampling::Both)) {

				if (i & 1) {
					chromaBase += chromaAdvance;
				}

			} else if constexpr (S == Subsampling::Vertical) {

				chromaBase += chromaAdvance;

			}

		}

	#endif

		return target.makeRaw();

	};

	u32 ySamplesX = scan.scanComponents[0].frameComponent.samplesX;
	u32 ySamplesY = scan.scanComponents[0].frameComponent.samplesY;
	u32 cbSamplesX = scan.scanComponents[1].frameComponent.samplesX;
	u32 cbSamplesY = scan.scanComponents[1].frameComponent.samplesY;
	u32 crSamplesX = scan.scanComponents[2].frameComponent.samplesX;
	u32 crSamplesY = scan.scanComponents[2].frameComponent.samplesY;

	u32 yArea = ySamplesX * ySamplesY;
	u32 cbArea = cbSamplesX * cbSamplesY;
	u32 crArea = crSamplesX * crSamplesY;

	if (Bool::all(yArea, cbArea, crArea, 1)) {

		//No chroma subsampling
		return exec.template operator()<Subsampling::None>();

	} else if (Bool::all(ySamplesX, ySamplesY, 2) && Bool::all(cbArea, crArea, 1)) {

		//Symmetrical chroma subsampling
		return exec.template operator()<Subsampling::Both>();

	} else if (yArea == 2 && Bool::all(cbArea, crArea, 1)) {

		if (ySamplesX == 1) {

			//Horizontally asymmetric chroma subsampling (H > V)
			return exec.template operator()<Subsampling::Horizontal>();

		} else {

			//Vertically asymmetric chroma subsampling (H < V)
			return exec.template operator()<Subsampling::Vertical>();

		}

	} else {

		//TODO: Generic upsampling
		throw UnsupportedOperationException("Subsampling variant not implemented yet");

	}

}


void JPEGDecoder::blendMonochrome() {
	image = blendMonochromeCore<fixTransformShift>(scan);
}



void JPEGDecoder::blendMonochromeTransformless() {
	image = blendMonochromeCore<0>(scan);
}



void JPEGDecoder::blendAndUpsampleYCbCr() {
	image = blendAndUpsampleYCbCrCore<fixTransformShift>(scan);
}



void JPEGDecoder::blendAndUpsampleYCbCrTransformless() {
	image = blendAndUpsampleYCbCrCore<0>(scan);
}



u16 JPEGDecoder::verifySegmentLength() {

	if (reader.remainingSize() < 2) {
		throw ImageDecoderException("Data size too small");
	}

	u16 length = reader.read<u16>();

	if (length < 2) {
		throw ImageDecoderException("Bad table length");
	}

	if (reader.remainingSize() < length - 2) {
		throw ImageDecoderException("Data size too small");
	}

	return length;

}


void JPEGDecoder::ArithmeticDecoder::reset() {

	EntropyDecoder::unblock();
	data = 0;
	size = 0;
	baseInterval = 0;

}



void JPEGDecoder::ArithmeticDecoder::prefetch() {

	for (u32 i = 0; i < 2; i++) {

		if (!sink.remainingSize()) {
			throw ImageDecoderException("Bad arithmetic stream");
		}

		u8 byte = sink.read<u8>();

		if (byte == 0xFF) {

			u8 nextByte = 0xFF;

			if (sink.remainingSize()) {
				nextByte = sink.read<u8>();
			}

			if (nextByte != 0) {

				//Marker shouldn't appear in the first two bytes
				throw ImageDecoderException("Bad arithmetic stream");

			}

		}

		data |= byte << 8;
		data <<= 8;

	}

}



bool JPEGDecoder::ArithmeticDecoder::decodeDCBin(ScanComponent& component, u32 bin) {
	return decodeBin(component.dcConditioning.bins[bin]);
}



bool JPEGDecoder::ArithmeticDecoder::decodeACBin(ScanComponent& component, u32 bin) {
	return decodeBin(component.acConditioning.bins[bin]);
}



bool JPEGDecoder::ArithmeticDecoder::decodeFixed(u16 lpsEstimate, bool mps) {

	baseInterval -= lpsEstimate;
	bool decision = false;

	u16 currentValue = getValue();

	if (currentValue < baseInterval) {

		if (baseInterval < 0x8000) {

			decision = baseInterval < lpsEstimate ? !mps : mps;
			renormalize();

		} else {

			decision = mps;

		}

	} else {

		setValue(currentValue - baseInterval);

		decision = baseInterval < lpsEstimate ? mps : !mps;
		baseInterval = lpsEstimate;

		renormalize();

	}

	return decision;


}



bool JPEGDecoder::ArithmeticDecoder::decodeBin(Bin& bin) {

	u16 lpsEstimate = arithmeticTransitionTable[bin.index].lpsEstimate;

	baseInterval -= lpsEstimate;
	bool decision = false;

	u16 currentValue = getValue();

	//True if we're in the MPS
	if (currentValue < baseInterval) {

		//Check if there is a need to renormalize
		if (baseInterval < 0x8000) {

			if (baseInterval < lpsEstimate) {

				decision = !bin.mps;
				lpsTransition(bin);

			} else {

				decision = bin.mps;
				mpsTransition(bin);

			}

			renormalize();

		} else {

			decision = bin.mps;

		}

	} else {

		//We're in the LPS
		setValue(currentValue - baseInterval);

		if (baseInterval < lpsEstimate) {

			decision = bin.mps;
			mpsTransition(bin);

		} else {

			decision = !bin.mps;
			lpsTransition(bin);

		}

		baseInterval = lpsEstimate;

		renormalize();

	}

	return decision;

}



void JPEGDecoder::ArithmeticDecoder::mpsTransition(Bin& bin) {
	bin.index = arithmeticTransitionTable[bin.index].nextMPS;
}



void JPEGDecoder::ArithmeticDecoder::lpsTransition(Bin& bin) {

	const ArithmeticStateTransition& ast = arithmeticTransitionTable[bin.index];

	if (ast.exchange) {
		bin.mps = !bin.mps;
	}

	bin.index = ast.nextLPS;

}



void JPEGDecoder::ArithmeticDecoder::renormalize() {

	u32 toShiftIn = Math::max(Bits::clz(baseInterval), 1);

	if (toShiftIn >= 16) {
		throw ImageDecoderException("Bad arithmetic stream");
	}

	baseInterval <<= toShiftIn;
	data <<= toShiftIn;

	if (end) {
		return;
	}

	u32 count = (toShiftIn + 7 - size) / 8;
	u32 newData = 0;
	u32 byteShift = toShiftIn + 8 - size;

	for (u32 i = 0; i < count; i++) {

		auto byte = fetchByte();

		if (!byte.has_value()) {
			break;
		}

		newData |= *byte << byteShift;
		byteShift -= 8;

	}

	data |= newData;
	size = (size + 16 - toShiftIn) % 8;

}



void JPEGDecoder::HuffmanDecoder::reset() {

	EntropyDecoder::unblock();
	data = 0;
	size = 0;

}



HuffmanResult JPEGDecoder::HuffmanDecoder::decodeDC(const JPEG::HuffmanTable& table) {

	u32 index = read(8);
	HuffmanResult result = table.fastTable[index];

	if (result.second == 0xFF) {

		consume(8);

		u32 extReadCount = table.maxLength - 8;
		result = table.extTables[result.first][read(extReadCount)];
		consume(result.second - 8);

	} else {

		consume(result.second);

	}

	return result;

}



HuffmanResult JPEGDecoder::HuffmanDecoder::decodeAC(const JPEG::HuffmanTable& table) {

	u32 index = read(8);
	HuffmanResult result = table.fastTable[index];

	if (result.second == 0xFF) {

		consume(8);

		u32 extReadCount = table.maxLength - 8;
		result = table.extTables[result.first][read(extReadCount)];
		consume(result.second - 8);

	} else {

		consume(result.second);

	}

	return result;

}



u32 JPEGDecoder::HuffmanDecoder::decodeOffset(u8 category) {

	u32 offset = read(category);
	consume(category);

	return offset;

}



void JPEGDecoder::HuffmanDecoder::saturate() {

	if (end) {
		return;
	}

	u32 count = (32 - size) / 8;

	for (u32 i = 0; i < count; i++) {

		auto byte = fetchByte();

		if (!byte.has_value()) {
			return;
		}

		data |= *byte << (32 - size - 8);
		size += 8;

	}

}



u32 JPEGDecoder::HuffmanDecoder::read(u32 count) {

	arc_assert(count <= 24, "Attempted to read more than 24 bits from buffer");

	if (count > size) {
		saturate();
	}

	return data >> (32 - count);

}



void JPEGDecoder::HuffmanDecoder::consume(u32 count) {

	size -= i32(count);
	data <<= count;

}



std::optional<u8> JPEGDecoder::EntropyDecoder::fetchByte() {

	if (!sink.remainingSize()) {

		end = true;
		return {};

	}

	u8 byte = sink.read<u8>();

	//Skip trailing 'escape zero'
	if (byte == 0xFF) {

		if (!sink.remainingSize()) {

			end = true;
			return {};

		}

		u8 nextByte = sink.read<u8>();

		if (nextByte) {

			//Restart intervals simply cause a buffer block
			if (nextByte < (Markers::RST0 & 0xFF) || nextByte > (Markers::RST7 & 0xFF)) {

				//Seek back to not consume the unknown marker
				if (nextByte != (Markers::DNL & 0xFF)) {
					sink.seek(-2);
				}

			}

			//Block stream
			end = true;
			return {};

		}

	}

	return byte;

}