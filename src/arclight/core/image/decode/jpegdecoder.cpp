/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 jpegdecoder.cpp
 */

#include "jpegdecoder.hpp"
#include "time/profiler.hpp"
#include "debug.hpp"


using namespace JPEG;


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
				table[i] = reader.read<T>();
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

	//Setup component data
	for (ImageComponent& component : scan.imageComponents) {

		component.width = (frame.samples * component.samplesX + scan.maxSamplesX - 1) / scan.maxSamplesX;
		component.height = (frame.lines * component.samplesY + scan.maxSamplesY - 1) / scan.maxSamplesY;

		u32 multipleX = component.samplesX * 8;
		u32 multipleY = component.samplesY * 8;
		u32 blocksPerMCU = component.samplesX * component.samplesY;

		scan.mcuDataUnits += blocksPerMCU;

		u32 mcusX = (component.width + multipleX - 1) / multipleX;
		u32 mcusY = (component.height + multipleY - 1) / multipleY;

		if (!scan.totalMCUs) {
			scan.totalMCUs = mcusX * mcusY;
		}

		component.dcLength = Bits::ctz(component.dcTable.size());
		component.acLength = Bits::ctz(component.acTable.size());

		component.blockData.resize(scan.totalMCUs * blocksPerMCU * 64);
		component.imageData.resize(component.width * component.height);

	}

	if (scan.mcuDataUnits > 10) {
		throw ImageDecoderException("Too many data units in MCU");
	}

	if (restartEnabled) {

		ArcDebug() << "Restart enabled";

	} else {

		decodeImage();

	}

	for (ImageComponent& component : scan.imageComponents) {

		for (u32 i = 0; i < component.blockData.size() / 64; i++) {

			ArcDebug() << std::span{component.blockData.data() + i * 64, 64};

		}

	}

}



void JPEGDecoder::decodeImage() {

	//Reset prediction
	for (ImageComponent& component : scan.imageComponents) {
		component.prediction = 0;
	}

	decodingBuffer.reset();

	for (u32 currentMCU = 0; currentMCU < scan.totalMCUs; currentMCU++) {

		for (ImageComponent& component : scan.imageComponents) {

			for (u32 sx = 0; sx < component.samplesX; sx++) {

				for (u32 sy = 0; sy < component.samplesY; sy++, component.dataUnit++) {

					decodeBlock(component);

				}

			}

		}

	}

}



void JPEGDecoder::decodeBlock(JPEG::ImageComponent& component) {

	SizeT baseDataOffset = component.dataUnit * 64;


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
			difference = offset >= (1 << (category - 1)) ? static_cast<i32>(offset) : -i32(((1 << category) - 1)) + static_cast<i32>(offset);
		}

		i32 dc = component.prediction + difference;
		component.prediction = dc;
		component.blockData[baseDataOffset] = dc;
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
			i32 ac = offset >= (1 << (category - 1)) ? Bits::cast<i32>(offset) : Bits::cast<i32>(-((1 << category) - 1) + offset);

			if (coefficient >= 64) {

				//Oh no
				Log::warn("JPEG Decoder", "AC symbol overflow, stream corrupted");
				break;

			}

			component.blockData[baseDataOffset + coefficient] = ac;
			coefficient++;

		}

	}

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