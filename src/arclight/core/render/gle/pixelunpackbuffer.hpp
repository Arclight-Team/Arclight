/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 pixelunpackbuffer.hpp
 */

#pragma once

#include "buffer.hpp"


GLE_BEGIN


class PixelUnpackBuffer : public Buffer {

public:

	constexpr PixelUnpackBuffer() : Buffer(BufferType::PixelUnpackBuffer) {}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::PixelUnpackBuffer);
	}

};


GLE_END