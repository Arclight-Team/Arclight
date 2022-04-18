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

	explicit JPEGDecoder(std::optional<Pixel> reqFormat) : IImageDecoder(reqFormat), validDecode(false), restartEnabled(false), decodingBuffer(reader), restartInterval(0) {}

	void decode(std::span<const u8> data);
	RawImage& getImage();

private:

	struct DecodingBuffer {

		constexpr explicit DecodingBuffer(BinaryReader& reader) : data(0), size(0), end(false), sink(reader) {}

		void reset();
		void saturate();
		u32 read(u32 count);
		void consume(u32 count);

		u32 data;
		i32 size;
		bool end;
		BinaryReader& sink;

	};

	void parseApplicationSegment0();
	void parseApplicationSegment1();
	void parseHuffmanTable();
	void parseQuantizationTable();
	void parseRestartInterval();

	void parseFrameHeader();
	void parseScanHeader();

	void resolveTargetFormat();

	void decodeScan();
	void decodeImage();
	void decodeBlock(JPEG::ImageComponent& component);

	static void applyIDCT(JPEG::ImageComponent& component, SizeT imageBase);
	static void applyPartialIDCT(JPEG::ImageComponent& component, SizeT imageBase, u32 width, u32 height);

	void blendAndUpsample();
	void blendMonochrome();
	void blendAndUpsampleYCbCr();

	u16 verifySegmentLength();

	Pixel baseFormat;

	JPEG::Scan scan;
	JPEG::Frame frame;
	JPEG::HuffmanTable dcHuffmanTables[4];
	JPEG::HuffmanTable acHuffmanTables[4];
	JPEG::QuantizationTable quantizationTables[4];

	bool restartEnabled;
	u32 restartInterval;

	BinaryReader reader;
	bool validDecode;

	DecodingBuffer decodingBuffer;
	RawImage image;

};