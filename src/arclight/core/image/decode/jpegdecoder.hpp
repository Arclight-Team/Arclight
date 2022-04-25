/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 jpegdecoder.hpp
 */

#pragma once

#include "jpeg.hpp"
#include "decoder.hpp"
#include "image/image.hpp"
#include "stream/binaryreader.hpp"
#include "math/matrix.hpp"
#include "debug.hpp"
#include "time/profiler.hpp"


class JPEGDecoder : public IImageDecoder {

public:

	explicit JPEGDecoder(std::optional<Pixel> reqFormat) : IImageDecoder(reqFormat), baseFormat(Pixel::RGB8), validDecode(false),
		restartEnabled(false), huffmanDecoder(reader), arithmeticDecoder(reader), restartInterval(0) {}

	void decode(std::span<const u8> data);
	RawImage& getImage();

private:

	struct HuffmanDecoder {

		constexpr explicit HuffmanDecoder(BinaryReader& reader) : data(0), size(0), sink(reader) {}

		void reset();
		JPEG::HuffmanResult decodeDC(const JPEG::HuffmanTable& table);
		JPEG::HuffmanResult decodeAC(const JPEG::HuffmanTable& table);
		u32 decodeOffset(u8 category);

		void saturate();
		u32 read(u32 count);
		void consume(u32 count);

		u32 data;
		i32 size;
		BinaryReader& sink;

	};

	struct ArithmeticDecoder {

		struct Bin {

			constexpr Bin() : index(0), mps(false) {}

			u32 index;
			bool mps;

		};

		explicit ArithmeticDecoder(BinaryReader& reader) : sink(reader) { reset(); }

		constexpr u16 getValue() const noexcept { return data >> 16; }
		constexpr void setValue(u16 value) noexcept { data = (data & 0xFFFF) | (value << 16); }

		void reset();
		void prefetch();
		bool decodeBin(Bin& bin);
		static void mpsTransition(Bin& bin);
		static void lpsTransition(Bin& bin);

		void renormalize();

		u16 baseInterval;
		std::array<Bin,  49> dcBins;
		std::array<Bin, 245> acBins;

		u32 data;
		u32 size;
		BinaryReader& sink;

	};

	void parseApplicationSegment0();
	void parseApplicationSegment1();
	void parseHuffmanTable();
	void parseArithmeticConditioning();
	void parseQuantizationTable();
	void parseRestartInterval();

	void parseFrameHeader();
	void parseScanHeader();

	void resolveTargetFormat();

	void decodeScan();
	void decodeImage();
	void decodeHuffmanBlock(JPEG::ScanComponent& component);
	void decodeArithmeticBlock(JPEG::ScanComponent& component);
	void decodeProgressiveDCBlock(JPEG::ScanComponent& component);

	i32* clearBlockBuffer(JPEG::ScanComponent& component);

	static void applyIDCT(JPEG::ScanComponent& component, SizeT imageBase);
	static void applyPartialIDCT(JPEG::ScanComponent& component, SizeT imageBase, u32 width, u32 height);

	void blendAndUpsample();
	void blendMonochrome();
	void blendAndUpsampleYCbCr();

	u16 verifySegmentLength();

	Pixel baseFormat;

	JPEG::Scan scan;
	JPEG::Frame frame;
	JPEG::HuffmanTable dcHuffmanTables[4];
	JPEG::HuffmanTable acHuffmanTables[4];
	JPEG::ArithmeticDCConditioning dcConditioning[4];
	JPEG::ArithmeticACConditioning acConditioning[4];
	JPEG::QuantizationTable quantizationTables[4];

	bool restartEnabled;
	u32 restartInterval;

	BinaryReader reader;
	bool validDecode;

	HuffmanDecoder huffmanDecoder;
	ArithmeticDecoder arithmeticDecoder;

	RawImage image;

};