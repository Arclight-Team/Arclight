/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 pixelpackbuffer.hpp
 */

#pragma once

#include "buffer.hpp"
#include "texture.hpp"
#include <GL/glew.h>


GLE_BEGIN


enum class TexturePackType {
	Texture1D,
	Texture2D,
	Texture3D,
	ArrayTexture1D,
	ArrayTexture2D,
	CubemapTexturePositiveX,
	CubemapTextureNegativeX,
	CubemapTexturePositiveY,
	CubemapTextureNegativeY,
	CubemapTexturePositiveZ,
	CubemapTextureNegativeZ,
	CubemapArrayTexture
};


class PixelPackBuffer : public Buffer {

public:

	constexpr PixelPackBuffer() : Buffer(BufferType::PixelPackBuffer) {}

	//Binds to the default target
	inline void bind() {
		Buffer::bind(BufferType::PixelPackBuffer);
	}

	void loadTexture(SizeT offset, TexturePackType type, TextureSourceFormat srcFormat, TextureSourceType srcType, u32 level = 0);

private:

	static bool isTextureBound(TexturePackType type);
	static u32 getTexturePackTypeEnum(TexturePackType type);

};


GLE_END