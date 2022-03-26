/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 decoder.hpp
 */

#pragma once

#include "types.hpp"
#include "util/concepts.hpp"

#include <span>
#include <stdexcept>



class IImageDecoder {};

template<class T>
concept ImageDecoder = BaseOf<IImageDecoder, T> && requires (T&& t, std::span<const u8>&& s) {
	t.decode(s);
};



class ImageDecoderException : public std::runtime_error {

public:
	explicit ImageDecoderException(const std::string& msg) : std::runtime_error(msg) {}

};