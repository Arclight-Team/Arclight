/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 encoder.hpp
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



class IImageEncoder {

public:

	constexpr explicit IImageEncoder(std::optional<Pixel> reqFormat) noexcept : requestedFormat(reqFormat) {}

	constexpr bool autoDetectFormat() const noexcept {
		return !requestedFormat.has_value();
	}

protected:

	std::optional<Pixel> requestedFormat;

};

namespace CC {

	template<class T>
	concept ImageEncoder = CC::BaseOf<T, IImageEncoder> && requires (T&& t, RawImage&& i) {
		t.encode(i);											//Encode function
		{ t.getBuffer() } -> CC::Equal<const std::vector<u8>&>;	//Buffer retrieval function
	};

}



class ImageEncoderException : public ArclightException {

public:
	using ArclightException::ArclightException;
	virtual const char* name() const noexcept override { return "Image Encoder Exception"; }

};