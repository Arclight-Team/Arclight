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


class JPEGDecoder : public IImageDecoder {

public:

	JPEGDecoder() : validDecode(false), restartEnabled(false), decodingBuffer(reader) {}

	void decode(std::span<const u8> data);

	template<Pixel P>
	Image<P> getImage() const {

		if (!validDecode) {
			throw ImageDecoderException("Bad image decode");
		}

		const JPEG::ImageComponent& component = scan.imageComponents[0];
		Image<P> image(component.width, component.height);

		constexpr Mat3f transform(1, 0, 1.402, 1, -0.344136, -0.714136, 1, 1.772, 0);

		for (u32 i = 0; i < image.getHeight(); i++) {

			for (u32 j = 0; j < image.getWidth(); j++) {

				SizeT offset = i * image.getWidth() + j;
				//Vec3i rgb = transform * (Vec3f(scan.imageComponents[0].imageData[offset], scan.imageComponents[1].imageData[offset], scan.imageComponents[2].imageData[offset]) + Vec3f(0, 128, 128));
				//image.setPixel(j, i, Image<P>::PixelType((rgb.x & 0xFF) << 16 | (rgb.y & 0xFF) << 8 | (rgb.z & 0xFF)));
				image.setPixel(j, i, Image<P>::PixelType(component.imageData[offset]));

			}

		}

		image.flipY();

		return image;

	}

private:

	struct DecodingBuffer {

		constexpr DecodingBuffer(BinaryReader& reader) : data(0), size(0), empty(false), sink(reader) {}

		void reset();
		void saturate(u32 reqSize);
		u32 read(u32 count);
		void consume(u32 count);

		u32 data;
		i32 size;
		bool empty;
		BinaryReader& sink;

	};

	void parseApplicationSegment0();
	void parseApplicationSegment1();
	void parseHuffmanTable();
	void parseQuantizationTable();
	void parseRestartInterval();

	void parseFrameHeader();
	void parseScanHeader();

	SizeT calculateScanSize();

	void decodeScan();
	void decodeImage();
	void decodeBlock(JPEG::ImageComponent& component);

	void applyIDCT(JPEG::ImageComponent& component, u32 mcu);

	u16 verifySegmentLength();

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

};