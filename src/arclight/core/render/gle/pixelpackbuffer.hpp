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
	Texture1D				= 0x0DE0, // GL_TEXTURE_1D
	Texture2D				= 0x0DE1, // GL_TEXTURE_2D
	Texture3D				= 0x806F, // GL_TEXTURE_3D
	ArrayTexture1D			= 0x8513, // GL_TEXTURE_CUBE_MAP
	ArrayTexture2D			= 0x8C18, // GL_TEXTURE_1D_ARRAY
	CubemapTexturePositiveX	= 0x8C1A, // GL_TEXTURE_2D_ARRAY
	CubemapTextureNegativeX	= 0x8515, // GL_TEXTURE_CUBE_MAP_POSITIVE_X
	CubemapTexturePositiveY	= 0x8516, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	CubemapTextureNegativeY	= 0x8517, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	CubemapTexturePositiveZ	= 0x8518, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	CubemapTextureNegativeZ	= 0x8519, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	CubemapArrayTexture		= 0x851A, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
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

};


GLE_END