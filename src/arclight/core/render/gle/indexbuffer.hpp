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