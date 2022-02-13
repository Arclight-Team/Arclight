/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 texturebuffer.hpp
 */

#pragma once

#include "buffer.hpp"
#include "texture.hpp"


GLE_BEGIN


class TextureBuffer : public Buffer {

public:

	constexpr TextureBuffer() : 
		Buffer(BufferType::TextureBuffer),
		format(TextureBufferFormat::RGBA8),
		texID(invalidID)
	{}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::TextureBuffer);
	}

	virtual bool create() override;
	virtual void destroy() override;

	void load(TextureBufferFormat format);
	void loadRange(SizeT offset, SizeT size, TextureBufferFormat format);

	void activate(u32 unit);

	void bindImageUnit(u32 unit, Access access);

private:

	TextureBufferFormat format;
	u32 texID;

};


GLE_END