/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 decoder.hpp
 */

#pragma once

#include "common/types.hpp"
#include "image/pixel.hpp"
#include "image/rawimage.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "common/exception.hpp"

#include <span>
#include <optional>
#include <stdexcept>



class IImageDecoder {

public:

	constexpr explicit IImageDecoder(std::optional<Pixel> reqFormat) noexcept : requestedFormat(reqFormat) {}

	constexpr bool autoDetectFormat() const noexcept {
		return !requestedFormat.has_value();
	}

protected:

	std::optional<Pixel> requestedFormat;

};

namespace CC {

	template<class T>
	concept ImageDecoder = CC::BaseOf<T, IImageDecoder> && requires (T&& t, std::span<const u8>&& s) {
		t.decode(s);								//Decode function
		{ t.getImage() } -> CC::Equal<RawImage&>;	//Image retrieval function
	};

}



class ImageDecoderException : public ArclightException {

public:
	using ArclightException::ArclightException;
	virtual const char* name() const noexcept override { return "Image Decoder Exception"; }

};