/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 IndexBuffer.hpp
 */

#pragma once

#include "Buffer.hpp"


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