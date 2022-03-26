/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 jpegdecoder.cpp
 */

#include "jpegdecoder.hpp"
#include "time/profiler.hpp"
#include "arcintrinsic.hpp"
#include "debug.hpp"


using namespace JPEG;


constexpr static u32 fixScaleShift = 10;
constexpr static u32 fixMultiplyShift = 10;
constexpr static u32 fixTransformShift = 8;
constexpr static i32 colorBias = 1 << (fixTransformShift - 1);

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

					SizeT scanSize = calculateScanSize();

					BinaryReader oldReader = reader;
					reader = reader.substream(scanSize);

					decodeScan();

					reader = oldReader;
					reader.seek(scanSize);
				}
				break;

			default:
				//Log::error("JPEG Loader", "Unknown marker 0x%X", marker);
				reader.seek(-1);
				break;

		}


		//Next marker
		if (reader.remainingSize() < 2) {
			throw ImageDecoderException("Bad stream end");
		}

		marker = reader.read<u16>();

	}


	validDecode = true;

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

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[APP0 JFIF] Major: %d, Minor: %d, Density: 0x%X, DensityX: %d, DensityY: %d, ThumbnailW: %d, ThumbnailH: %d",
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

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[APP0 JFXX] ThumbnailFormat: 0x%X", format);
#endif

		switch (format) {

			case 0x10:
				//TODO: Embedded JPEG
				Log::warn("JPEG Loader", "Embedded JPEG thumbnails unsupported");
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
		Log::warn("JPEG Decoder", "Unknown APP0 segment found: %s", app0String);
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

	//Setting + L1-16
	u32 offset = 19;
	u32 count = 0;

	do {

		if (length < offset) {
			throw ImageDecoderException("[DHT] Bad table length");
		}

		u8 settings = reader.read<u8>();
		u8 type = settings >> 4;
		u8 id = settings & 0xF;

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[DHT] Class: %d, ID: %d", type, id);
#endif

		if (type > 1 || id > 3) {
			throw ImageDecoderException("[DHT] Bad table settings");
		}

		std::array<u8, 16> codeCounts{};
		u32 totalCodeCount = 0;
		u32 highestLength = 1;

		for (u32 i = 0; i < 16; i++) {

			u8 codeCount = reader.read<u8>();

			codeCounts[i] = codeCount;
			totalCodeCount += codeCount;

			if (codeCount) {
				highestLength = i;
			}

		}

		offset += totalCodeCount;

		if (length < offset) {
			throw ImageDecoderException("[DHT] Bad table length");
		}

		HuffmanTable& table = type == 0 ? dcHuffmanTables[id] : acHuffmanTables[id];

		std::vector<u8> symbols(totalCodeCount);
		reader.read(std::span {symbols.data(), symbols.size()});

		table.resize(2 << highestLength);

		u32 index = 0;
		u32 code = 0;

		//For each code length
		for (u32 i = 0; i < 16; i++) {

			//For each code
			for (u32 j = 0; j < codeCounts[i]; j++) {

				//Fast huffman code must be reversed
				u32 startCode = Bits::reverse(code) >> (31 - i);

				//Fill fast huffman table
				for (u32 k = startCode; k < table.size(); k += 2 << i) {
					table[k] = {symbols[index], i + 1};
				}

				code++;
				index++;

			}

			code <<= 1;

		}

		count++;

	} while (offset != length);

#ifdef ARC_IMAGE_DEBUG
	Log::info("JPEG Loader", "[DHT] Tables read: %d", count);
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

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", std::string("[DQT] Table %d, ") + (precision ? "16 bit" : "8 bit"), id);
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

			for (u32 i = 0; i < 64; i++) {
				table[i] = static_cast<i32>(reader.read<T>()) * scaleFactors[dezigzagTable[i]];
			}

		};

		if (precision) {
			loadQTable(id, u16{});
		} else {
			loadQTable(id, u8{});
		}

		count++;

	} while (offset != length);

#ifdef ARC_IMAGE_DEBUG
	Log::info("JPEG Loader", "[DQT] Tables read: %d", count);
#endif

}



void JPEGDecoder::parseRestartInterval() {

	u16 length = verifySegmentLength();

	if (length != 4) {
		throw ImageDecoderException("[DRI] Bad table size");
	}

	restartInterval = reader.read<u16>();
	restartEnabled = restartInterval;

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

#ifdef ARC_IMAGE_DEBUG
	Log::info("JPEG Loader", "[SOF] Bits: %d, Lines: %d, Samples: %d, Components: %d", frame.bits, frame.lines, frame.samples, components);
#endif

	switch (frame.type) {

		case FrameType::Baseline:

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

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[SOF] Component: %d, SX: %d, SY: %d, QTableIndex: %d", id, component.sx, component.sy, q);
#endif

		if (!Math::inRange(component.sx, 1, 4) || !Math::inRange(component.sy, 1, 4) || q > 3 || (frame.type == FrameType::Lossless && q != 0)) {
			throw ImageDecoderException("[SOF] Bad component data");
		}

		if (frame.components.contains(id)) {
			throw ImageDecoderException("[SOF] Duplicate component ID");
		}

		frame.components.emplace(id, component);

	}

}



void JPEGDecoder::parseScanHeader() {

	u16 length = verifySegmentLength();

	if (length < 3) {
		throw ImageDecoderException("[SOS] Bad length");
	}

	u8 components = reader.read<u8>();

#ifdef ARC_IMAGE_DEBUG
	Log::info("JPEG Loader", "[SOS] Images: %d", components);
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

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[SOS] Component %d: ID: %d, DCID: %d, ACID: %d", i + 1, componentID, dcTableID, acTableID);
#endif

		if (!frame.components.contains(componentID)) {
			throw ImageDecoderException("[SOS] No frame component found matching scan component index");
		}

		FrameComponent& frameComponent = frame.components[componentID];

		scan.maxSamplesX = Math::max(scan.maxSamplesX, frameComponent.sx);
		scan.maxSamplesY = Math::max(scan.maxSamplesY, frameComponent.sy);

		if (dcTableID > 3 || (frame.type == FrameType::Baseline && dcTableID > 1)) {
			throw ImageDecoderException("[SOS] Illegal DC table ID");
		}

		if (dcTableID > 3 || (frame.type == FrameType::Baseline && dcTableID > 1) || (frame.type == FrameType::Lossless && dcTableID)) {
			throw ImageDecoderException("[SOS] Illegal AC table ID");
		}

		if (dcHuffmanTables[dcTableID].empty() || acHuffmanTables[acTableID].empty()) {
			throw ImageDecoderException("[SOS] Huffman table not installed");
		}

		if (quantizationTables[frameComponent.qID].empty()) {
			throw ImageDecoderException("[SOS] Quantization table not installed");
		}

		scan.imageComponents.emplace_back(dcHuffmanTables[dcTableID], acHuffmanTables[acTableID], quantizationTables[frameComponent.qID], frameComponent.sx, frameComponent.sy);

	}

	scan.spectralStart = reader.read<u8>();
	scan.spectralEnd = reader.read<u8>();

	u8 approx = reader.read<u8>();
	scan.approximationHigh = approx >> 4;
	scan.approximationLow = approx & 0xF;

#ifdef ARC_IMAGE_DEBUG
	Log::info("JPEG Loader", "[SOS] SStart: %d, SEnd: %d, ApproxHigh: %d, ApproxLow: %d", scan.spectralStart, scan.spectralEnd, scan.approximationHigh, scan.approximationLow);
#endif

	switch (frame.type) {

		case FrameType::Baseline:
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

			break;

		case FrameType::Lossless:

			if (scan.spectralStart > 7 || scan.spectralEnd || scan.approximationHigh || scan.approximationLow > 15) {
				throw ImageDecoderException("[SOS] Bad scan settings");
			}

			break;

	}

}



SizeT JPEGDecoder::calculateScanSize() {

	const u8* data = reader.head();
	SizeT maxSize = reader.remainingSize();

	for (SizeT i = 0; i < maxSize; i++) {

		if (data[i] == 0xFF && (i + 1) < maxSize && data[i + 1] != 0) {
			return i;
		}

	}

	return maxSize;

}



void JPEGDecoder::decodeScan() {

	//Start scan decoding
	if (frame.type != FrameType::Baseline) {
		Log::error("JPEG Decoder", "Decoder can only decode baseline JPEG");
		return;
	}

	if (!reader.size()) {
		throw ImageDecoderException("Scan empty");
	}

	scan.mcuDataUnits = 0;
	scan.totalMCUs = 0;

	ARC_PROFILE_START(ScanSetup)

	//Setup component data
	for (ImageComponent& component : scan.imageComponents) {

		component.width = (frame.samples * component.samplesX + scan.maxSamplesX - 1) / scan.maxSamplesX;
		component.height = (frame.lines * component.samplesY + scan.maxSamplesY - 1) / scan.maxSamplesY;

		u32 multipleX = component.samplesX * 8;
		u32 multipleY = component.samplesY * 8;
		u32 blocksPerMCU = component.samplesX * component.samplesY;

		scan.mcuDataUnits += blocksPerMCU;

		if (!scan.totalMCUs) {

			scan.mcusX = (component.width + multipleX - 1) / multipleX;
			scan.mcusY = (component.height + multipleY - 1) / multipleY;
			scan.totalMCUs = scan.mcusX * scan.mcusY;

		}

		component.dcLength = Bits::ctz(component.dcTable.size());
		component.acLength = Bits::ctz(component.acTable.size());

		component.imageData.resize(component.width * component.height);

	}

	ARC_PROFILE_STOP(ScanSetup)

	if (scan.mcuDataUnits > 10) {
		throw ImageDecoderException("Too many data units in MCU");
	}

	if (restartEnabled) {

		ArcDebug() << "Restart enabled";

	} else {

		decodeImage();

	}

	blendAndUpsample();

}



void JPEGDecoder::decodeImage() {

	alignas(32) i32 block[64];

	//Reset prediction and block buffer
	for (ImageComponent& component : scan.imageComponents) {

		component.prediction = 0;
		component.block = block;

	}

	decodingBuffer.reset();

	ARC_PROFILE_START(Decode)

	for (u32 currentMCU = 0; currentMCU < scan.totalMCUs; currentMCU++) {

		for (ImageComponent& component : scan.imageComponents) {

			SizeT mcuBaseX = (currentMCU % scan.mcusX) * component.samplesX * 8;
			SizeT mcuBaseY = (currentMCU / scan.mcusX) * component.samplesY * 8;

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

						decodeBlock(component);
						applyPartialIDCT(component, baseY * component.width + baseX, w, h);

					}

				} else {

					for (u32 sx = 0; sx < component.samplesX; sx++) {

						SizeT baseX = mcuBaseX + sx * 8;

						decodeBlock(component);

						if (baseX + 8 > component.width) {
							applyPartialIDCT(component, baseY * component.width + baseX, component.width - baseX, 8);
						} else {
							applyIDCT(component, baseY * component.width + baseX);
						}

					}

				}

			}

		}

	}

	ARC_PROFILE_STOP(Decode)

}



void JPEGDecoder::decodeBlock(JPEG::ImageComponent& component) {

	i32* block = std::assume_aligned<32>(component.block);

	//This really shouldn't be necessary, but MSVC fails to vectorize std::fill_n
#ifdef ARC_VECTORIZE_X86_AVX2

	__m256i* ptr = reinterpret_cast<__m256i*>(block);

	for (u32 i = 0; i < 8; i++) {
		_mm256_store_si256(ptr++, _mm256_setzero_si256());
	}

#elif defined(ARC_VECTORIZE_X86_SSE2)

	__m128i* ptr = reinterpret_cast<__m128i*>(block);

	for (u32 i = 0; i < 16; i++) {
		_mm_store_si128(ptr++, _mm_setzero_si128());
	}

#else

	std::fill_n(block, 64, 0);

#endif

	//DC
	{
		u32 index = decodingBuffer.read(component.dcLength);
		auto result = component.dcTable[index];

		decodingBuffer.consume(result.second);

		u32 category = result.first;
		u32 offset = Bits::reverse(decodingBuffer.read(category)) >> (32 - category);

		decodingBuffer.consume(category);

		i32 difference = 0;

		if (category) {
			difference = offset >= (1 << (category - 1)) ? static_cast<i32>(offset) : -i32((1 << category) - 1) + static_cast<i32>(offset);
		}

		i32 dc = component.prediction + difference;
		component.prediction = dc;
		block[0] = dc * component.qTable[0];
	}


	//AC
	u32 coefficient = 1;

	while (coefficient < 64) {

		u32 index = decodingBuffer.read(component.acLength);
		auto result = component.acTable[index];

		decodingBuffer.consume(result.second);

		u8 symbol = result.first;
		u8 category = symbol & 0xF;
		u8 zeroes = symbol >> 4;

		if (category == 0) {

			//Special values
			if (zeroes == 0) {

				//End Of Block
				return;

			} else if (zeroes == 0xF) {

				//Zero Run Length
				coefficient += 16;

			} else {

				Log::warn("JPEG Decoder", "Bad AC symbol, skipping");
				coefficient++;

			}

		} else {

			//Extract the AC magnitude
			u32 offset = Bits::reverse(decodingBuffer.read(category)) >> (32 - category);
			decodingBuffer.consume(category);

			coefficient += zeroes;

			//Calculate the AC value
			i32 ac = offset >= (1 << (category - 1)) ? static_cast<i32>(offset) : -i32((1 << category) - 1) + static_cast<i32>(offset);

			if (coefficient >= 64) {

				//Oh no
				Log::warn("JPEG Decoder", "AC symbol overflow, stream corrupted");
				break;

			}

			u32 dezigzagIndex = dezigzagTableTransposed[coefficient];
			block[dezigzagIndex] = ac * component.qTable[coefficient];

			coefficient++;

		}

	}

}



template<bool Full>
constexpr static void scalarIDCT(const i32* inData, i32* outData, SizeT outStride, u32 width, u32 height) {

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
			outData[outStride * i + j] = (tmp[j] >> (fixScaleShift - fixTransformShift)) + (128 << fixTransformShift);
		}

	}

}



void JPEGDecoder::applyIDCT(JPEG::ImageComponent& component, SizeT imageBase) {

	const i32* inData = component.block;            //Aligned to 32 bytes
	i32* outData = &component.imageData[imageBase];

#ifdef ARC_VECTORIZE_X86_AVX2

	const __m256i* inVec = reinterpret_cast<const __m256i*>(inData);

	__m256i bufVec[8];

	__m256i m0 = _mm256_set1_epi32(multiplyConstants[0]);
	__m256i m1 = _mm256_set1_epi32(multiplyConstants[1]);
	__m256i m2 = _mm256_set1_epi32(multiplyConstants[2]);
	__m256i m3 = _mm256_set1_epi32(128 << fixTransformShift);

	__m256i* outVec[8];

	for (u32 i = 0; i < 8; i++) {
		outVec[i] = reinterpret_cast<__m256i*>(outData + component.width * i);
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

		__m256i d0 = _mm256_add_epi32(c0, c7);
		__m256i d1 = _mm256_add_epi32(c1, c6);
		__m256i d2 = _mm256_add_epi32(c2, c5);
		__m256i d3 = _mm256_add_epi32(c3, c4);
		__m256i d4 = _mm256_sub_epi32(c3, c4);
		__m256i d5 = _mm256_sub_epi32(c2, c5);
		__m256i d6 = _mm256_sub_epi32(c1, c6);
		__m256i d7 = _mm256_sub_epi32(c0, c7);

		_mm256_storeu_si256(outVec[0], _mm256_add_epi32(_mm256_srai_epi32(d0, (fixScaleShift - fixTransformShift)), m3));
		_mm256_storeu_si256(outVec[1], _mm256_add_epi32(_mm256_srai_epi32(d1, (fixScaleShift - fixTransformShift)), m3));
		_mm256_storeu_si256(outVec[2], _mm256_add_epi32(_mm256_srai_epi32(d2, (fixScaleShift - fixTransformShift)), m3));
		_mm256_storeu_si256(outVec[3], _mm256_add_epi32(_mm256_srai_epi32(d3, (fixScaleShift - fixTransformShift)), m3));
		_mm256_storeu_si256(outVec[4], _mm256_add_epi32(_mm256_srai_epi32(d4, (fixScaleShift - fixTransformShift)), m3));
		_mm256_storeu_si256(outVec[5], _mm256_add_epi32(_mm256_srai_epi32(d5, (fixScaleShift - fixTransformShift)), m3));
		_mm256_storeu_si256(outVec[6], _mm256_add_epi32(_mm256_srai_epi32(d6, (fixScaleShift - fixTransformShift)), m3));
		_mm256_storeu_si256(outVec[7], _mm256_add_epi32(_mm256_srai_epi32(d7, (fixScaleShift - fixTransformShift)), m3));
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

	for (u32 i = 0; i < 2; i++) {

		__m128i in0 = _mm_load_si128(bufVec + i +  0);
		__m128i in1 = _mm_load_si128(bufVec + i +  2);
		__m128i in2 = _mm_load_si128(bufVec + i +  4);
		__m128i in3 = _mm_load_si128(bufVec + i +  6);
		__m128i in4 = _mm_load_si128(bufVec + i +  8);
		__m128i in5 = _mm_load_si128(bufVec + i + 10);
		__m128i in6 = _mm_load_si128(bufVec + i + 12);
		__m128i in7 = _mm_load_si128(bufVec + i + 14);

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

		//Untransposed thanks to transposed dezigzag
		_mm_storeu_si128(outVec[0] + i, _mm_add_epi32(_mm_srai_epi32(d0, (fixScaleShift - fixTransformShift)), m3));
		_mm_storeu_si128(outVec[1] + i, _mm_add_epi32(_mm_srai_epi32(d1, (fixScaleShift - fixTransformShift)), m3));
		_mm_storeu_si128(outVec[2] + i, _mm_add_epi32(_mm_srai_epi32(d2, (fixScaleShift - fixTransformShift)), m3));
		_mm_storeu_si128(outVec[3] + i, _mm_add_epi32(_mm_srai_epi32(d3, (fixScaleShift - fixTransformShift)), m3));
		_mm_storeu_si128(outVec[4] + i, _mm_add_epi32(_mm_srai_epi32(d4, (fixScaleShift - fixTransformShift)), m3));
		_mm_storeu_si128(outVec[5] + i, _mm_add_epi32(_mm_srai_epi32(d5, (fixScaleShift - fixTransformShift)), m3));
		_mm_storeu_si128(outVec[6] + i, _mm_add_epi32(_mm_srai_epi32(d6, (fixScaleShift - fixTransformShift)), m3));
		_mm_storeu_si128(outVec[7] + i, _mm_add_epi32(_mm_srai_epi32(d7, (fixScaleShift - fixTransformShift)), m3));

	}

#else

	scalarIDCT<true>(inData, outData, component.width, 8, 8);

#endif

}



void JPEGDecoder::applyPartialIDCT(JPEG::ImageComponent& component, SizeT imageBase, u32 width, u32 height) {
	scalarIDCT<false>(component.block, &component.imageData[imageBase], component.width, width, height);
}



void JPEGDecoder::blendAndUpsample() {

	const JPEG::ImageComponent& component = scan.imageComponents[0];
	Image<Pixel::RGB8> target(component.width, component.height);

	const i32* imgData[3] = {scan.imageComponents[0].imageData.data(),
	                         scan.imageComponents[1].imageData.data(),
	                         scan.imageComponents[2].imageData.data()};

	ARC_PROFILE_START(CoreBlend)


#ifdef ARC_VECTORIZE_X86_SSE4_1

	SizeT totalPixels = target.getWidth() * target.getHeight();
	SizeT vectorSize = totalPixels / 16;
	SizeT scalarSize = totalPixels % 16;

	__m128i rcr = _mm_set1_epi32(22970);
	__m128i gcb = _mm_set1_epi32(5638);
	__m128i gcr = _mm_set1_epi32(11700);
	__m128i bcb = _mm_set1_epi32(29032);
	__m128i csb = _mm_set1_epi32(128 << fixTransformShift);
	__m128i bias = _mm_set1_epi32(colorBias);

	__m128i shuf0 = _mm_setr_epi32(0x02010F0B, 0x07060503, 0x0C0A0908, 0x04000E0D);
	__m128i shuf1 = _mm_setr_epi32(0x01080400, 0x06020905, 0x0B07030A, 0x0D0F0E0C);
	__m128i shuf2 = _mm_setr_epi32(0x030E0502, 0x07040F06, 0x09000B08, 0x0D0A010C);
	__m128i shuf3 = _mm_setr_epi32(0x03010002, 0x050C0804, 0x0A060D09, 0x0F0B070E);

	__m128i* targetVecData = reinterpret_cast<__m128i*>(target.getImageBuffer().data());
	const __m128i* vecData[3] = { reinterpret_cast<const __m128i*>(imgData[0]),
	                              reinterpret_cast<const __m128i*>(imgData[1]),
	                              reinterpret_cast<const __m128i*>(imgData[2]) };

	for (SizeT i = 0; i < vectorSize; i++) {

		__m128i y0  = _mm_load_si128(vecData[0]);
		__m128i cb0 = _mm_sub_epi32(_mm_load_si128(vecData[1]), csb);
		__m128i cr0 = _mm_sub_epi32(_mm_load_si128(vecData[2]), csb);
		__m128i y1  = _mm_load_si128(vecData[0] + 1);
		__m128i cb1 = _mm_sub_epi32(_mm_load_si128(vecData[1] + 1), csb);
		__m128i cr1 = _mm_sub_epi32(_mm_load_si128(vecData[2] + 1), csb);
		__m128i y2  = _mm_load_si128(vecData[0] + 2);
		__m128i cb2 = _mm_sub_epi32(_mm_load_si128(vecData[1] + 2), csb);
		__m128i cr2 = _mm_sub_epi32(_mm_load_si128(vecData[2] + 2), csb);
		__m128i y3  = _mm_load_si128(vecData[0] + 3);
		__m128i cb3 = _mm_sub_epi32(_mm_load_si128(vecData[1] + 3), csb);
		__m128i cr3 = _mm_sub_epi32(_mm_load_si128(vecData[2] + 3), csb);

		__m128i rx0 = _mm_srai_epi32(_mm_mullo_epi32(cr0, rcr), 14);
		__m128i gx0 = _mm_srai_epi32(_mm_mullo_epi32(cb0, gcb), 14);
		__m128i gy0 = _mm_srai_epi32(_mm_mullo_epi32(cr0, gcr), 14);
		__m128i bx0 = _mm_srai_epi32(_mm_mullo_epi32(cb0, bcb), 14);
		__m128i rx1 = _mm_srai_epi32(_mm_mullo_epi32(cr1, rcr), 14);
		__m128i gx1 = _mm_srai_epi32(_mm_mullo_epi32(cb1, gcb), 14);
		__m128i gy1 = _mm_srai_epi32(_mm_mullo_epi32(cr1, gcr), 14);
		__m128i bx1 = _mm_srai_epi32(_mm_mullo_epi32(cb1, bcb), 14);
		__m128i rx2 = _mm_srai_epi32(_mm_mullo_epi32(cr2, rcr), 14);
		__m128i gx2 = _mm_srai_epi32(_mm_mullo_epi32(cb2, gcb), 14);
		__m128i gy2 = _mm_srai_epi32(_mm_mullo_epi32(cr2, gcr), 14);
		__m128i bx2 = _mm_srai_epi32(_mm_mullo_epi32(cb2, bcb), 14);
		__m128i rx3 = _mm_srai_epi32(_mm_mullo_epi32(cr3, rcr), 14);
		__m128i gx3 = _mm_srai_epi32(_mm_mullo_epi32(cb3, gcb), 14);
		__m128i gy3 = _mm_srai_epi32(_mm_mullo_epi32(cr3, gcr), 14);
		__m128i bx3 = _mm_srai_epi32(_mm_mullo_epi32(cb3, bcb), 14);

		__m128i r0 = _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(y0, rx0), bias), fixTransformShift);
		__m128i g0 = _mm_srai_epi32(_mm_add_epi32(_mm_sub_epi32(_mm_sub_epi32(y0, gx0), gy0), bias), fixTransformShift);
		__m128i b0 = _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(y0, bx0), bias), fixTransformShift);
		__m128i r1 = _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(y1, rx1), bias), fixTransformShift);
		__m128i g1 = _mm_srai_epi32(_mm_add_epi32(_mm_sub_epi32(_mm_sub_epi32(y1, gx1), gy1), bias), fixTransformShift);
		__m128i b1 = _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(y1, bx1), bias), fixTransformShift);
		__m128i r2 = _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(y2, rx2), bias), fixTransformShift);
		__m128i g2 = _mm_srai_epi32(_mm_add_epi32(_mm_sub_epi32(_mm_sub_epi32(y2, gx2), gy2), bias), fixTransformShift);
		__m128i b2 = _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(y2, bx2), bias), fixTransformShift);
		__m128i r3 = _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(y3, rx3), bias), fixTransformShift);
		__m128i g3 = _mm_srai_epi32(_mm_add_epi32(_mm_sub_epi32(_mm_sub_epi32(y3, gx3), gy3), bias), fixTransformShift);
		__m128i b3 = _mm_srai_epi32(_mm_add_epi32(_mm_add_epi32(y3, bx3), bias), fixTransformShift);

		__m128i m0 = _mm_packus_epi32(r0, g0);      //r0, r1, r2, r3, g0, g1, g2, g3
		__m128i m1 = _mm_packus_epi32(b0, r1);
		__m128i m2 = _mm_packus_epi32(g1, b1);
		__m128i m3 = _mm_packus_epi32(r2, g2);
		__m128i m4 = _mm_packus_epi32(b2, r3);
		__m128i m5 = _mm_packus_epi32(g3, b3);

		__m128i m6 = _mm_packus_epi16(m0, m1);      //r0, r1, r2, r3, g0, g1, g2, g3, b0, b1, b2, b3, r4, r5, [r6, r7]
		__m128i m7 = _mm_packus_epi16(m2, m3);      //[g4], g5, g6, g7, [b4], b5, b6, b7, r8, r9, rA, [rB], g8, g9, gA, [gB]
		__m128i m8 = _mm_packus_epi16(m4, m5);      //[b8, b9], bA, bB, rC, rD, rE, rF, gC, gD, gE, gF, bC, bD, bE, bF

		__m128i n0 = _mm_shuffle_epi8(m7, shuf0);                               //rB, gB, g5, g6, g7, b5, b6, b7, r8, r9, rA, g8, g9, gA, g4, b4

		__m128i n1 = _mm_blend_epi16(m6, n0, 0x80);                             //r0, r1, r2, r3, g0, g1, g2, g3, b0, b1, b2, b3, r4, r5, g4, b4
		__m128i n2 = _mm_blend_epi16(_mm_blend_epi16(n0, m6, 0x80), m8, 0x01);  //b8, b9, g5, g6, g7, b5, b6, b7, r8, r9, rA, g8, g9, gA, r6, r7
		__m128i n3 = _mm_blend_epi16(m8, n0, 0x01);                             //rB, gB, bA, bB, rC, rD, rE, rF, gC, gD, gE, gF, bC, bD, bE, bF

		__m128i c0 = _mm_shuffle_epi8(n1, shuf1);   //r0, g0, b0, r1, g1, b1, r2, g2, b2, r3, g3, b3, r4, g4, b4, r5
		__m128i c1 = _mm_shuffle_epi8(n2, shuf2);   //g5, b5, r6, g6, b6, r7, g7, b7, r8, g8, b8, r9, g9, b9, rA, gA
		__m128i c2 = _mm_shuffle_epi8(n3, shuf3);   //bA, rB, gB, bB, rC, gC, bC, rD, gD, bD, rE, gE, bE, rF, gF, bF

		_mm_storeu_si128(targetVecData + 0, c0);
		_mm_storeu_si128(targetVecData + 1, c1);
		_mm_storeu_si128(targetVecData + 2, c2);

		targetVecData += 3;

		for (u32 j = 0; j < 3; j++) {
			vecData[j] += 4;
		}

	}

	u8* targetSclData = Bits::toByteArray(targetVecData);

	for (u32 i = 0; i < 3; i++) {
		imgData[i] = reinterpret_cast<const i32*>(vecData[i]);
	}

	for (SizeT i = 0; i < scalarSize; i++) {

		//YCbCr to RGB
		i32 y  = *imgData[0];
		i32 cb = *imgData[1] - (128 << fixTransformShift);
		i32 cr = *imgData[2] - (128 << fixTransformShift);

		i32 r = y + ((cr * 22970) >> 14);
		i32 g = y - ((cb * 5638) >> 14) - ((cr * 11700) >> 14);
		i32 b = y + ((cb * 29032) >> 14);

		u8 rb = Math::clamp((r + colorBias) >> fixTransformShift, 0, 255);
		u8 gb = Math::clamp((g + colorBias) >> fixTransformShift, 0, 255);
		u8 bb = Math::clamp((b + colorBias) >> fixTransformShift, 0, 255);

		targetSclData[0] = rb;
		targetSclData[1] = gb;
		targetSclData[2] = bb;

		targetSclData += 3;

		for (u32 j = 0; j < 3; j++) {
			imgData[j]++;
		}

	}

#else

	SizeT offset = 0;

	for (u32 i = 0; i < target.getHeight(); i++) {

		for (u32 j = 0; j < target.getWidth(); j++) {

			//YCbCr to RGB
			i32 y = imgData[0][offset];
			i32 cb = imgData[1][offset] - (128 << fixTransformShift);
			i32 cr = imgData[2][offset] - (128 << fixTransformShift);

			i32 r = y + ((cr * 22970) >> 14);
			i32 g = y - ((cb * 5638) >> 14) - ((cr * 11700) >> 14);
			i32 b = y + ((cb * 29032) >> 14);

			target.setPixel(j, i, PixelRGB8(Math::clamp((r + colorBias) >> fixTransformShift, 0, 255), Math::clamp((g + colorBias) >> fixTransformShift, 0, 255), Math::clamp((b + colorBias) >> fixTransformShift, 0, 255)));

			offset++;

		}

	}

#endif


	image = target.makeRaw();
	ARC_PROFILE_STOP(CoreBlend)

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



void JPEGDecoder::DecodingBuffer::reset() {

	data = 0;
	size = 0;
	empty = false;

}



void JPEGDecoder::DecodingBuffer::saturate(u32 reqSize) {

	arc_assert(reqSize <= 24, "Attempted to over-saturate decoding buffer");

	while (size < reqSize) {

		if (sink.remainingSize()) {

			u8 byte = sink.read<u8>();

			//Skip trailing 'escape zero'
			//Safe because the pre-scan stopped at the first non-zero FF-sequence
			if (byte == 0xFF) {

				u8 nextByte = sink.read<u8>();

				if (nextByte != 0) {

					//Marker found, must be restart interval
					//TODO

				}

			}

			data |= Bits::reverse(byte) << size;
			size += 8;

		} else {

			empty = true;
			return;

		}

	}

}



u32 JPEGDecoder::DecodingBuffer::read(u32 count) {

	if (count > size) {
		saturate(count);
	}

	return Bits::mask(data, 0, count);

}



void JPEGDecoder::DecodingBuffer::consume(u32 count) {

	size -= i32(count);
	data >>= count;

}