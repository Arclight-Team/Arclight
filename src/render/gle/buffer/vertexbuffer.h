#pragma once

#include "buffer.h"


GLE_BEGIN


class VertexBuffer : public Buffer {

public:

	constexpr VertexBuffer() {}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::VertexBuffer);
	}

};


GLE_END