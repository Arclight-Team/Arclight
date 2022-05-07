/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ppmdecoder.cpp
 */

#include "ppmdecoder.hpp"
#include "debug.hpp"



void PPMDecoder::decode(std::span<const u8> data) {

	validDecode = false;

	std::string stringData(Bits::rcast<const char*>(data.data()), data.size());
	std::istringstream stringReader(stringData);
	
	constexpr u32 FetchMagic		= 0;
	constexpr u32 FetchWidth		= 1;
	constexpr u32 FetchHeight		= 2;
	constexpr u32 FetchColorRange	= 3;
	constexpr u32 ParseRasterData	= 4;

	u32 parseStep = FetchMagic;

	i32 width;
	i32 height;
	i32 colorRange;

	while (true) {

		std::string string;

		if (stringReader.eof())
			throw ImageDecoderException("Premature end of data");

		// Skip comments
		if (stringReader.peek() == '#') {
			stringReader >> string;
			continue;
		}

		// Proceed to reading the raw RGB8 data
		if (parseStep == ParseRasterData)
			break;

		switch (parseStep) {

		case FetchMagic:

			// Fetch magic string
			stringReader >> string;

			if (string != "P6")
				throw ImageDecoderException("PPM magic doesn't match");

			parseStep++;

			break;

		case FetchWidth:

			// Fetch image width
			stringReader >> width;

			if (stringReader.fail())
				throw ImageDecoderException("Failed to fetch width");

			if (width <= 0)
				throw ImageDecoderException("Width should be greater than zero");

			parseStep++;

			break;

		case FetchHeight:

			// Fetch image height
			stringReader >> height;

			if (stringReader.fail())
				throw ImageDecoderException("Failed to fetch height");

			if (height <= 0)
				throw ImageDecoderException("Height should be greater than zero");

			parseStep++;

			break;

		case FetchColorRange:

			// Fetch color range
			stringReader >> colorRange;

			if (stringReader.fail())
				throw ImageDecoderException("Failed to fetch color range");

			if (colorRange < 0 || colorRange >= 65536)
				throw ImageDecoderException("Color range should be in range 0-65535");

			parseStep++;

			break;

		}

	}

	Image<Pixel::RGB8> bufImage(width, height);
	u64 requiredSize = bufImage.pixelCount() * 3;

	u64 currentPos = 1 + stringReader.tellg();
	u64 remainingSize = data.size() - currentPos;

	if (remainingSize < requiredSize)
		throw ImageDecoderException("Not enough bytes for raster data");

	bufImage.setRawData(data.subspan(currentPos, requiredSize));
	
	image = bufImage.makeRaw();
	validDecode = true;

}



RawImage& PPMDecoder::getImage() {

	if (!validDecode) {
		throw ImageDecoderException("Bad image decode");
	}

	return image;

}
