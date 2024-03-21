/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 TGADecoder.hpp
 */

#pragma once

#include "Decoder.hpp"
#include "Image/Image.hpp"
#include "Stream/BinaryReader.hpp"
#include <array>



class TGADecoder : public IImageDecoder {

public:

	constexpr explicit TGADecoder(std::optional<Pixel> reqFormat) noexcept : IImageDecoder(reqFormat), validDecode(false) {}

	void decode(std::span<const u8> data);
	RawImage& getImage();

	constexpr auto& getID() const {
		return id;
	}

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

	std::vector<u8> id;
	Vec2us origin;
	
};
