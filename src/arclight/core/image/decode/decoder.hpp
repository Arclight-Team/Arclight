/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 decoder.hpp
 */

#pragma once

#include "util/concepts.hpp"

#include <stdexcept>



class IImageDecoder {};

template<class T>
concept ImageDecoder = BaseOf<IImageDecoder, T>;



class ImageDecoderException : public std::runtime_error {

public:
	explicit ImageDecoderException(const std::string& msg) : std::runtime_error(msg) {}

};