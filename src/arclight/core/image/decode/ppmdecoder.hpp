/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ppmdecoder.hpp
 */

#pragma once

#include "decoder.hpp"
#include "bitmap.hpp"
#include "image/image.hpp"
#include "stream/binaryreader.hpp"



class PPMDecoder : public IImageDecoder {

public:

    constexpr explicit PPMDecoder(std::optional<Pixel> reqFormat) noexcept : IImageDecoder(reqFormat), validDecode(false) {}

    void decode(std::span<const u8> data);
	RawImage& getImage();

private:

    RawImage image;
    bool validDecode;

};
