/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 indexbuffer.hpp
 */

#pragma once

#include "buffer.hpp"


GLE_BEGIN


class IndexBuffer : public Buffer {

public:

	constexpr IndexBuffer() : Buffer(BufferType::ElementBuffer) {}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::ElementBuffer);
	}

};


GLE_END