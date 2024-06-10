/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 VertexBuffer.hpp
 */

#pragma once

#include "Buffer.hpp"


GLE_BEGIN


class VertexBuffer : public Buffer {

public:

	constexpr VertexBuffer() : Buffer(BufferType::VertexBuffer) {}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::VertexBuffer);
	}

};


GLE_END