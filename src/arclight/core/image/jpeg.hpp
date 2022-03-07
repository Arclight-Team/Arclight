/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 jpeg.hpp
 */

#pragma once

#include "image.hpp"
#include "filesystem/file.hpp"
#include "stream/binaryreader.hpp"
#include "arcconfig.hpp"
#include "debug.hpp"
#include "time/profiler.hpp"

#include <unordered_map>



class JPEGDecoderException : public std::runtime_error {

public:
	template<class... Args> JPEGDecoderException(const std::string& msg, Args&&... args) : std::runtime_error(String::format(msg, std::forward<Args>(args)...)) {}

};



namespace JPEG {

	constexpr u8 jfifString[5] = {0x4A, 0x46, 0x49, 0x46, 0x00};
	constexpr u8 jfxxString[5] = {0x4A, 0x46, 0x58, 0x58, 0x00};

	namespace Markers {

		constexpr u16 SOI = 0xFFD8;
		constexpr u16 EOI = 0xFFD9;
		constexpr u16 APP0 = 0xFFE0;
		constexpr u16 DQT = 0xFFDB;
		constexpr u16 SOF0 = 0xFFC0;
		constexpr u16 SOF1 = 0xFFC1;
		constexpr u16 SOF2 = 0xFFC2;
		constexpr u16 SOF3 = 0xFFC3;
		constexpr u16 SOF5 = 0xFFC5;
		constexpr u16 SOF6 = 0xFFC6;
		constexpr u16 SOF7 = 0xFFC7;
		constexpr u16 JPG = 0xFFC8;
		constexpr u16 SOF9 = 0xFFC9;
		constexpr u16 SOF10 = 0xFFCA;
		constexpr u16 SOF11 = 0xFFCB;
		constexpr u16 SOF13 = 0xFFCD;
		constexpr u16 SOF14 = 0xFFCE;
		constexpr u16 SOF15 = 0xFFCF;
		constexpr u16 DHT = 0xFFC4;
		constexpr u16 SOS = 0xFFDA;

	};


	enum class FrameType {
		Baseline,
		ExtendedSequential,
		Progressive,
		Lossless
	};

	enum class Encoding {
		Huffman,
		Arithmetic
	};

	struct Component {

		constexpr Component() noexcept : Component(0, 0, 0) {}
		constexpr Component(u8 sx, u8 sy, u8 q) noexcept : sx(sx), sy(sy), qIndex(q) {}

		u8 sx, sy;
		u8 qIndex;

	};

	struct Frame {

		Frame() : type(FrameType::Baseline), differential(false), encoding(Encoding::Huffman), bits(8), lines(0), samples(1) {}

		FrameType type;
		bool differential;
		Encoding encoding;

		u32 bits;
		u32 lines;
		u32 samples;

		std::unordered_map<u8, Component> components;

	};

	struct ImageComponent {

		constexpr ImageComponent() noexcept : component(0), dcTableID(0), acTableID(0) {}

		u8 component;
		u8 dcTableID;
		u8 acTableID;

	};

	struct Scan {

		constexpr Scan() noexcept : spectralStart(0), spectralEnd(0), approximationHigh(0), approximationLow(0) {}

		std::vector<ImageComponent> imageComponents;
		u32 spectralStart;
		u32 spectralEnd;
		u32 approximationHigh;
		u32 approximationLow;

	};


	using QuantizationTable = std::vector<u32>;
	using HuffmanTable = std::vector<u8>;

	struct DecodingContext {

		Scan scan;
		Frame frame;
		HuffmanTable dcHuffmanTables[4];
		HuffmanTable acHuffmanTables[4];
		QuantizationTable quantizationTables[4];

	};


	constexpr bool app0StringCompare(const u8* a, const u8* b) noexcept {

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

	constexpr u16 verifySegmentLength(BinaryReader& reader) {

		if (reader.remainingSize() < 2) {
			throw JPEGDecoderException("Data size too small");
		}

		u16 length = reader.read<u16>();

		if (length < 2) {
			throw JPEGDecoderException("Bad table length");
		}

		if (reader.remainingSize() < length - 2) {
			throw JPEGDecoderException("Data size too small");
		}

		return length;

	}

	void dispatchHuffmanTable(DecodingContext& context, BinaryReader& reader);
	void dispatchQuantizationTable(DecodingContext& context, BinaryReader& reader);
	void dispatchFrameHeader(DecodingContext& context, BinaryReader& reader);
	void dispatchScanHeader(DecodingContext& context, BinaryReader& reader);

	SizeT calculateScanSize(const BinaryReader& reader);
	void scan(const DecodingContext& context, BinaryReader reader);


	template<Pixel P>
	Image<P> loadInternal(const std::span<const u8>& data) {

		using Format = PixelFormat<P>;
		using PixelType = typename PixelType<P>::Type;

		Profiler profiler;
		profiler.start();

		BinaryReader reader(data, ByteOrder::Big);

		//The file must at least fit SOI / APP0 marker
		if (reader.remainingSize() < 6) {
			throw JPEGDecoderException("Data size too small");
		}

		//Start by looking for SOI
		u16 soiMarker = reader.read<u16>();

		if (soiMarker != Markers::SOI) {
			throw JPEGDecoderException("No SOI marker found");
		}


		bool foundAPP0 = false;
		u16 marker = reader.read<u16>();

		while (marker == Markers::APP0) {

			u16 length = verifySegmentLength(reader);
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
				throw JPEGDecoderException("Bad APP0 type string");
			}

			if (app0StringCompare(app0String, jfifString)) {

				//JFIF APP0

				if (length < 16) {
					throw JPEGDecoderException("Bad APP0 segment length of %d", length);
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
					throw JPEGDecoderException("Illegal APP0 segment data");
				}

				u32 thumbnailBytes = thumbnailW * thumbnailH * 3;

				if (length != thumbnailBytes + 16) {
					throw JPEGDecoderException("Bad APP0 length of %d", length);
				}

				Image<Pixel::RGB8> thumbnail;
				thumbnail.setRawData(std::span {reader.head(), thumbnailBytes});

				foundAPP0 = true;

			} else if (app0StringCompare(app0String, jfxxString)) {

				//JFXX APP0

				//Shared + SOI / Thumbnail dims
				if (length < 8 + 2) {
					throw JPEGDecoderException("Bad APP0 extension segment length of %d", length);
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
									throw JPEGDecoderException("Bad APP0 extension length of %d", length);
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
									throw JPEGDecoderException("Bad APP0 extension length of %d", length);
								}

								thumbnail.setRawData(std::span {reader.head(), thumbnailPixels * 3});

							}

						}
						break;

					default:
						throw JPEGDecoderException("Bad APP0 extension thumbnail format");

				}

			} else {

				//Unknown, skip
				reader.seekTo(segmentEnd);

			}

			if (reader.remainingSize() < 2) {
				throw JPEGDecoderException("Bad stream end");
			}

			marker = reader.read<u16>();

		}

		if (!foundAPP0) {
			throw JPEGDecoderException("Failed to find APP0 marker");
		}


		DecodingContext context;


		while (marker != Markers::EOI) {

			switch (marker) {

				case Markers::DQT:
					dispatchQuantizationTable(context, reader);
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

					context.frame.type = static_cast<FrameType>(marker & 0x03);
					context.frame.differential = marker & 0x04;
					context.frame.encoding = static_cast<Encoding>((marker & 0x08) >> 3);

					dispatchFrameHeader(context, reader);

					break;

				case Markers::DHT:
					dispatchHuffmanTable(context, reader);
					break;

				case Markers::SOS:
					{
						dispatchScanHeader(context, reader);

						SizeT scanSize = calculateScanSize(reader);
						scan(context, reader.substream(scanSize));

						reader.seek(scanSize);
					}
					break;

				default:
					//Log::error("JPEG Loader", "Unknown marker 0x%X", marker);
					break;

			}

			if (reader.remainingSize() < 2) {
				throw JPEGDecoderException("Data size too small");
			}

			marker = reader.read<u16>();

		}

		profiler.stop("JPEG");

		return Image<P>();

	}



	void dispatchHuffmanTable(DecodingContext& context, BinaryReader& reader) {

		u16 length = verifySegmentLength(reader);

		FrameType frameType = context.frame.type;

		//Setting + L1-16
		u32 offset = 19;
		u32 count = 0;

		do {

			if (length < offset) {
				throw JPEGDecoderException("[DHT] Bad table length");
			}

			u8 settings = reader.read<u8>();

			u8 type = settings >> 4;
			u8 id = settings & 0xF;

#ifdef ARC_IMAGE_DEBUG
			Log::info("JPEG Loader", "[DHT] Class: %d, ID: %d", type, id);
#endif

			if (type > 1 || id > 3) {
				throw JPEGDecoderException("[DHT] Bad table settings");
			}

			std::array<u8, 16> codeCounts;
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
				throw JPEGDecoderException("[DHT] Bad table length");
			}

			HuffmanTable& table = type == 0 ? context.dcHuffmanTables[id] : context.acHuffmanTables[id];

			std::vector<u8> symbols(totalCodeCount);
			reader.read(std::span {table.data(), table.size()});

			table.resize(2 << highestLength);

			u32 index = 0;
			u32 code = 0;

			//For each code length
			for (u32 i = 0; i < 16; i++, code <<= 1) {

				//For each code
				for (u32 j = 0; j < codeCounts[i]; j++, code++) {

					u32 curIndex = index++;

					//Fill fast huffman table
					for (u32 k = code; k < table.size(); k += 2 << i) {
						table[k] = symbols[curIndex];
					}

				}

			}

			count++;

		} while (offset != length);

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[DHT] Tables read: %d", count);
#endif

	}



	template<Arithmetic A>
	void dispatchQuantizationTableSingle(QuantizationTable& table, BinaryReader& reader) {

		for (u32 i = 0; i < 64; i++) {
			table[i] = reader.read<A>();
		}

	}



	void dispatchQuantizationTable(DecodingContext& context, BinaryReader& reader) {

		u16 length = verifySegmentLength(reader);

		u32 offset = 2;
		u32 count = 0;

		do {

			if (length < offset + 1) {
				throw JPEGDecoderException("[DQT] Bad table length");
			}

			u8 setting = reader.read<u8>();
			u8 precision = setting >> 4;
			u8 id = setting & 0xF;

#ifdef ARC_IMAGE_DEBUG
			Log::info("JPEG Loader", std::string("[DQT] Table %d, ") + (precision ? "16 bit" : "8 bit"), id);
#endif

			if (precision > 1 || id > 3) {
				throw JPEGDecoderException("[DQT] Illegal quantization setting 0x%X", setting);
			}

			offset += 65 + precision * 64;

			if (length < offset) {
				throw JPEGDecoderException("[DQT] Table too small");
			}

			QuantizationTable& table = context.quantizationTables[id];
			table.resize(64);

			if (precision) {
				dispatchQuantizationTableSingle<u16>(table, reader);
			} else {
				dispatchQuantizationTableSingle<u8>(table, reader);
			}

			count++;

		} while (offset != length);

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[DQT] Tables read: %d", count);
#endif

	}



	void dispatchFrameHeader(DecodingContext& context, BinaryReader& reader) {

		u16 length = verifySegmentLength(reader);

		if (length < 8) {
			throw JPEGDecoderException("[SOF] Bad length");
		}

		Frame& frame = context.frame;

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
					throw JPEGDecoderException("[SOF] Bit depth for baseline must be 8");
				}

				break;

			case FrameType::ExtendedSequential:

				if (frame.bits != 8 && frame.bits != 12) {
					throw JPEGDecoderException("[SOF] Bit depth for extended sequential must be 8 or 12");
				}

				break;

			case FrameType::Progressive:

				if (frame.bits != 8 && frame.bits != 12) {
					throw JPEGDecoderException("[SOF] Bit depth for progressive must be 8 or 12");
				}

				if (components > 4) {
					throw JPEGDecoderException("[SOF] Progressive cannot have more than 4 components");
				}

				break;

			case FrameType::Lossless:

				if (!Math::inRange(frame.bits, 2, 16)) {
					throw JPEGDecoderException("[SOF] Bit depth for lossless must be between 2 and 16");
				}

				break;

		}

		if (components == 0) {
			throw JPEGDecoderException("[SOF] Component count may not be 0");
		}

		if (frame.samples == 0) {
			throw JPEGDecoderException("[SOF] Line sample count may not be 0");
		}

		if (length < components * 3 + 8) {
			throw JPEGDecoderException("[SOF] Bad length");
		}

		for (u32 i = 0; i < components; i++) {

			u8 id = reader.read<u8>();
			u8 s = reader.read<u8>();
			u8 q = reader.read<u8>();

			Component component(s >> 4, s & 0xF, q);

#ifdef ARC_IMAGE_DEBUG
			Log::info("JPEG Loader", "[SOF] Component: %d, SX: %d, SY: %d, QTableIndex: %d", id, component.sx, component.sy, q);
#endif

			if (!Math::inRange(component.sx, 1, 4) || !Math::inRange(component.sy, 1, 4) || q > 3 || (frame.type == FrameType::Lossless && q != 0)) {
				throw JPEGDecoderException("[SOF] Bad component data");
			}

			if (frame.components.contains(id)) {
				throw JPEGDecoderException("[SOF] Duplicate component ID %d", id);
			}

			frame.components.emplace(id, component);

		}

	}



	void dispatchScanHeader(DecodingContext& context, BinaryReader& reader) {

		u16 length = verifySegmentLength(reader);

		if (length < 3) {
			throw JPEGDecoderException("[SOS] Bad length");
		}

		u8 components = reader.read<u8>();

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[SOS] Images: %d", components);
#endif

		if (!Math::inRange(components, 1, 4)) {
			throw JPEGDecoderException("[SOS] Illegal component count");
		}

		if (length < components * 2 + 6) {
			throw JPEGDecoderException("[SOS] Bad length");
		}

		Scan& scan = context.scan;
		Frame& frame = context.frame;
		FrameType frameType = frame.type;

		u32 samplingSum = 0;

		for (u32 i = 0; i < components; i++) {

			ImageComponent imgComp;
			imgComp.component = reader.read<u8>();

			u8 ids = reader.read<u8>();
			imgComp.dcTableID = ids >> 4;
			imgComp.acTableID = ids & 0xF;

			scan.imageComponents.emplace_back(imgComp);

#ifdef ARC_IMAGE_DEBUG
			Log::info("JPEG Loader", "[SOS] Component %d: ID: %d, DCID: %d, ACID: %d", i + 1, imgComp.component, imgComp.dcTableID, imgComp.acTableID);
#endif

			if (!frame.components.contains(imgComp.component)) {
				throw JPEGDecoderException("[SOS] No frame component found matching scan component index");
			}

			Component& c = frame.components[imgComp.component];
			samplingSum += c.sx * c.sy;

			if (imgComp.dcTableID > 3 || (frameType == FrameType::Baseline && imgComp.dcTableID > 1)) {
				throw JPEGDecoderException("[SOS] Illegal DC table ID");
			}

			if (imgComp.dcTableID > 3 || (frameType == FrameType::Baseline && imgComp.dcTableID > 1) || (frameType == FrameType::Lossless && imgComp.dcTableID)) {
				throw JPEGDecoderException("[SOS] Illegal AC table ID");
			}

		}

		if (samplingSum > 10) {
			throw JPEGDecoderException("[SOS] Sampling condition not satisfied");
		}

		scan.spectralStart = reader.read<u8>();
		scan.spectralEnd = reader.read<u8>();

		u8 approx = reader.read<u8>();
		scan.approximationHigh = approx >> 4;
		scan.approximationLow = approx & 0xF;

#ifdef ARC_IMAGE_DEBUG
		Log::info("JPEG Loader", "[SOS] SStart: %d, SEnd: %d, ApproxHigh: %d, ApproxLow: %d", scan.spectralStart, scan.spectralEnd, scan.approximationHigh, scan.approximationLow);
#endif

		switch (frameType) {

			case FrameType::Baseline:
			case FrameType::ExtendedSequential:

				if (scan.spectralStart != 0 || scan.spectralEnd != 63 || scan.approximationHigh != 0 || scan.approximationLow != 0) {
					throw JPEGDecoderException("[SOS] Bad scan settings");
				}

				break;

			case FrameType::Progressive:

				if (scan.spectralStart > 63 || scan.spectralEnd > 63 || scan.spectralStart > scan.spectralEnd
					|| (!scan.spectralStart && scan.spectralEnd) || scan.approximationHigh > 13 || scan.approximationLow > 13) {
					throw JPEGDecoderException("[SOS] Bad scan settings");
				}

				break;

			case FrameType::Lossless:

				if (scan.spectralStart > 7 || scan.spectralEnd || scan.approximationHigh || scan.approximationLow > 15) {
					throw JPEGDecoderException("[SOS] Bad scan settings");
				}

				break;

		}

	}



	SizeT calculateScanSize(const BinaryReader& reader) {

		const u8* data = reader.head();
		SizeT maxSize = reader.remainingSize();

		for (SizeT i = 0; i < maxSize; i++) {

			if (data[i] == 0xFF && (i + 1) < maxSize && data[i + 1] != 0) {
				return i;
			}

		}

		return maxSize;

	}



	void scan(const DecodingContext& context, BinaryReader reader) {

		//Start scan decoding

	}



	template<Pixel P>
	Image<P> load(const std::span<const u8>& data) {

		try {

			return loadInternal<P>(data);

		} catch (const JPEGDecoderException& e) {

			Log::error("JPEG Decoder", e.what());
			return Image<P>();

		}

	}


	template<Pixel P>
	Image<P> load(const Path& filepath) {

		File file(filepath);

		if (!file.open()) {
			Log::error("JPEG Loader", "Failed to load file %s", filepath.toString().c_str());
			return Image<P>();
		}

		std::vector<u8> bytes = file.readAll();

		return load<P>(bytes);

	}

}