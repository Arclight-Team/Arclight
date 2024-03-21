/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 QOIDecoder.hpp
 */

#pragma once

#include "Decoder.hpp"
#include "Image/Image.hpp"
#include "Stream/BinaryReader.hpp"



class QOIDecoder : public IImageDecoder {

public:

	constexpr explicit QOIDecoder(std::optional<Pixel> reqFormat) noexcept : IImageDecoder(reqFormat), validDecode(false) {}

	void decode(std::span<const u8> data);
	RawImage& getImage();

private:

	BinaryReader reader;
	RawImage image;
	bool validDecode;

};