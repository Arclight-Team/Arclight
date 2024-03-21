/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 PPMDecoder.hpp
 */

#pragma once

#include "Decoder.hpp"
#include "Image/Image.hpp"



class PPMDecoder : public IImageDecoder {

public:

	constexpr explicit PPMDecoder(std::optional<Pixel> reqFormat) noexcept : IImageDecoder(reqFormat), validDecode(false) {}

	void decode(std::span<const u8> data);
	RawImage& getImage();

private:

	RawImage image;
	bool validDecode;

};
