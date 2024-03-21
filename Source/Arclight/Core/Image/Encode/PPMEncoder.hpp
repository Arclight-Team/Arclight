/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 PPMEncoder.hpp
 */

#pragma once

#include "Encoder.hpp"
#include "Image/Image.hpp"



class PPMEncoder : public IImageEncoder {

public:

	constexpr explicit PPMEncoder(std::optional<Pixel> reqFormat) noexcept : IImageEncoder(reqFormat), validEncode(false) {}

	void encode(const RawImage& image);
	const std::vector<u8>& getBuffer();

private:

	std::vector<u8> buffer;
	bool validEncode;

};
