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

		return Image<P>::fromRaw(bitmap.image);

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