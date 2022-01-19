/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 uniformbuffer.hpp
 */

#pragma once

#include "buffer.hpp"


GLE_BEGIN


class UniformBuffer : public Buffer {

public:

	constexpr UniformBuffer() : Buffer(BufferType::UniformBuffer) {}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::UniformBuffer);
	}

	bool bindRange(u32 index, SizeT offset, SizeT size);

};


GLE_END