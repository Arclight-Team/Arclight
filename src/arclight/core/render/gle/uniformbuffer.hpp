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

	bool bindRange(u32 index, u32 offset, u32 size);

};


GLE_END