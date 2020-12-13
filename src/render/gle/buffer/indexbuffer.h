#pragma once

#include "buffer.h"


GLE_BEGIN


class IndexBuffer : public Buffer {

public:

	constexpr IndexBuffer() {}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::ElementBuffer);
	}

};


GLE_END