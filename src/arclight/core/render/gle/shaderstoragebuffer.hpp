/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 shaderstoragebuffer.hpp
 */

#pragma once

#include "buffer.hpp"


GLE_BEGIN


class ShaderStorageBuffer : public Buffer {

public:

	inline ShaderStorageBuffer() : Buffer(BufferType::ShaderStorageBuffer) {}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::ShaderStorageBuffer);
	}

	bool bindRange(u32 index, SizeT offset, SizeT size);

};


GLE_END