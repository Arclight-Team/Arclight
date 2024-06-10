/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 PPMEncoder.cpp
 */

#include "Image/Encode/PPMEncoder.hpp"
#include "Stream/BinaryWriter.hpp"



void PPMEncoder::encode(const RawImage& image) {

	validEncode = false;

	std::ostringstream stringWriter;

	u32 width = image.getWidth();
	u32 height = image.getHeight();
	u32 colorRange = 255;

	if (width == 0)
		throw ImageEncoderException("Image width should be non-zero");

	if (height == 0)
		throw ImageEncoderException("Image height should be non-zero");

	// Write header
	stringWriter << "P6\n";

	// Write width and height
	stringWriter << width << ' ' << height << '\n';

	// Write color range
	stringWriter << colorRange << '\n';

	SizeT headerSize = stringWriter.tellp();
	SizeT rasterSize = width * height * 3;
	
	buffer.resize(headerSize + rasterSize);
	BinaryWriter writer(buffer);
	
	// Write header
	writer.write<const char>(stringWriter.view());

	// Write raster data
	RawImage copy(image);
	RawImage rgb8 = Image<Pixel::RGB8>::fromRaw(copy, true).makeRaw();
	writer.write<const u8>(rgb8.getRawBuffer());

	validEncode = true;

}



const std::vector<u8>& PPMEncoder::getBuffer() {

	if (!validEncode) {
		throw ImageEncoderException("Bad image encode");
	}

	return buffer;

}
