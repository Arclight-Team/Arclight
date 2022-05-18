/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 tgadecoder.hpp
 */

#pragma once

#include "decoder.hpp"
#include "image/image.hpp"
#include "stream/binaryreader.hpp"



class TGADecoder : public IImageDecoder {

public:

	constexpr explicit TGADecoder(std::optional<Pixel> reqFormat) noexcept : IImageDecoder(reqFormat), validDecode(false) {}

	void decode(std::span<const u8> data);
	RawImage& getImage();

	constexpr Vec2us getOrigin() const {
		return origin;
	}

private:

	void readImageDataRLE(struct TGAHeader const&);
	void parseColorMapImageData(struct TGAHeader const&);
	void parseTrueColorImageData(struct TGAHeader const&);
	void parseBlackWhiteImageData(struct TGAHeader const&);

	BinaryReader reader;
	RawImage image;
	bool validDecode;

	std::vector<u8> colorMapData;
	std::vector<u8> imageData;

	Vec2us origin;

};
