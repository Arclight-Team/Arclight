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

	struct EntropyDecoder {

		constexpr explicit EntropyDecoder(BinaryReader& reader) : end(false), sink(reader) {}

		constexpr void unblock() noexcept { end = false; }
		std::optional<u8> fetchByte();

		bool end;
		BinaryReader& sink;

	};

	struct HuffmanDecoder : public EntropyDecoder {

		constexpr explicit HuffmanDecoder(BinaryReader& reader) : EntropyDecoder(reader), data(0), size(0) {}

		void reset();
		JPEG::HuffmanResult decodeDC(const JPEG::HuffmanTable& table);
		JPEG::HuffmanResult decodeAC(const JPEG::HuffmanTable& table);
		u32 decodeOffset(u8 category);

		void saturate();
		u32 read(u32 count);
		void consume(u32 count);

		u32 data;
		i32 size;

	};

	struct ArithmeticDecoder : public EntropyDecoder {

		constexpr explicit ArithmeticDecoder(BinaryReader& reader) : EntropyDecoder(reader), baseInterval(0), data(0), size(0) {}

		constexpr u16 getValue() const noexcept { return data >> 16; }
		constexpr void setValue(u16 value) noexcept { data = (data & 0xFFFF) | (value << 16); }

		void reset();
		void prefetch();
		bool decodeBin(JPEG::Bin& bin);
		bool decodeDCBin(JPEG::ScanComponent& component, u32 bin);
		bool decodeACBin(JPEG::ScanComponent& component, u32 bin);
		bool decodeFixed(u16 lpsEstimate, bool mps);
		static void mpsTransition(JPEG::Bin& bin);
		static void lpsTransition(JPEG::Bin& bin);

		void renormalize();

		u16 baseInterval;
		u32 data;
		u32 size;

	};

	void parseApplicationSegment0();
	void parseApplicationSegment1();
	void parseHuffmanTable();
	void parseArithmeticConditioning();
	void parseQuantizationTable();
	void parseRestartInterval();
	void parseComment();
	void parseNumberOfLines();

	void parseFrameHeader();
	void parseScanHeader();

	void searchForLineSegment();
	void resolveTargetFormat();

	void decodeScan();
	void decodeImage(u32 startMCU, u32 endMCU);
	void decodeHuffmanBlock(JPEG::ScanComponent& component);
	void decodeArithmeticBlock(JPEG::ScanComponent& component);
	void decodeProgressiveDCBlock(JPEG::ScanComponent& component);
	void predictSample(JPEG::ScanComponent& component, u32 x, u32 y, u32 predictor);
	i32 calculatePrediction(JPEG::ScanComponent& component, u32 x, u32 y, u32 predictor);
	static i16 sampleComponent(JPEG::ScanComponent& component, u32 x, u32 y);

	static i32* clearBlockBuffer(JPEG::ScanComponent& component);

	static void applyIDCT(JPEG::ScanComponent& component, SizeT imageBase);
	static void applyPartialIDCT(JPEG::ScanComponent& component, SizeT imageBase, u32 width, u32 height);

	void blendAndUpsample();
	void blendMonochrome();
	void blendMonochromeTransformless();
	void blendAndUpsampleYCbCr();
	void blendAndUpsampleYCbCrTransformless();

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

	U8String comment;
	HuffmanDecoder huffmanDecoder;
	ArithmeticDecoder arithmeticDecoder;

	RawImage image;

};