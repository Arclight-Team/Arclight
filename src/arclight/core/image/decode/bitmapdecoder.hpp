/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitmapdecoder.hpp
 */

#pragma once

#include "decoder.hpp"
#include "bitmap.hpp"
#include "image/image.hpp"
#include "stream/binaryreader.hpp"



class BitmapDecoder : public IImageDecoder {

public:

	BitmapDecoder() : validDecode(false) {}

	void decode(std::span<const u8> data);

	template<Pixel P>
	Image<P> getImage() const {

		if (!validDecode) {
			throw ImageDecoderException("Bad image decode");
		}

		switch (bitmap.compression) {

			case Bitmap::Compression::RLE4:
			case Bitmap::Compression::RLE8:
			case Bitmap::Compression::Masked:
				return Image<Pixel::BGRA8>::fromRaw(bitmap.image).convert<P>();

			case Bitmap::Compression::None:

				if (bitmap.bitsPerPixel < 16 || bitmap.bitsPerPixel == 32) {
					return Image<Pixel::BGRA8>::fromRaw(bitmap.image).convert<P>();
				} else if (bitmap.bitsPerPixel == 24) {
					return Image<Pixel::BGR8>::fromRaw(bitmap.image).convert<P>();
				} else {
					return Image<Pixel::BGR5>::fromRaw(bitmap.image).convert<P>();
				}

				break;

			default:
				ARC_UNREACHABLE;

		}

	}

private:

	SizeT parseHeader();
	void parseInfoHeader();

	void loadPalette();

	void decodeDirect();
	void decodeIndexed();
	void decodeRLE();
	void decodeMasked();

	BinaryReader reader;
	Bitmap bitmap;
	std::vector<PixelBGRA8> palette;
	bool validDecode;

};