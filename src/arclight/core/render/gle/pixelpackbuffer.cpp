/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 pixelpackbuffer.cpp
 */

#include "pixelpackbuffer.hpp"

#include "glecore.hpp"
#include GLE_HEADER


GLE_BEGIN


void PixelPackBuffer::loadTexture(SizeT offset, TexturePackType type, TextureSourceFormat srcFormat, TextureSourceType srcType, u32 level) {

	gle_assert(isBound(), "Pixel pack buffer %d has not been bound (attempted to load texture)", id);
	gle_assert(!isMapped(), "Cannot load texture to mapped pixel pack buffer (pixel pack buffer ID=%d)", id);
	gle_assert(isTextureBound(type), "Cannot load texture from not bound corresponding texture (pixel pack buffer ID=%d)", id);

	glGetTexImage(getTexturePackTypeEnum(type), level, Texture::getTextureSourceFormatEnum(srcFormat), Texture::getTextureSourceTypeEnum(srcType), reinterpret_cast<void*>(offset));
}



bool PixelPackBuffer::isTextureBound(TexturePackType type) {

	TextureType texType;

	switch (type) {

		case TexturePackType::Texture1D:
			texType = TextureType::Texture1D;
			break;

		case TexturePackType::Texture2D:
			texType = TextureType::Texture2D;
			break;

		case TexturePackType::Texture3D:
			texType = TextureType::Texture3D;
			break;

		case TexturePackType::ArrayTexture1D:
			texType = TextureType::ArrayTexture1D;
			break;

		case TexturePackType::ArrayTexture2D:
			texType = TextureType::ArrayTexture2D;
			break;

		case TexturePackType::CubemapTexturePositiveX:
		case TexturePackType::CubemapTextureNegativeX:
		case TexturePackType::CubemapTexturePositiveY:
		case TexturePackType::CubemapTextureNegativeY:
		case TexturePackType::CubemapTexturePositiveZ:
		case TexturePackType::CubemapTextureNegativeZ:
			texType = TextureType::CubemapTexture;
			break;

		case TexturePackType::CubemapArrayTexture:
			texType = TextureType::CubemapArrayTexture;
			break;

		default:
			gle_force_assert("Invalid texture pack type 0x%X", type);
			return false;

	}

	return Texture::getBoundTextureID(texType) != invalidBoundID;
}



u32 PixelPackBuffer::getTexturePackTypeEnum(TexturePackType type) {

	switch (type) {

		case TexturePackType::Texture1D:
			return GL_TEXTURE_1D;

		case TexturePackType::Texture2D:
			return GL_TEXTURE_2D;

		case TexturePackType::Texture3D:
			return GL_TEXTURE_3D;

		case TexturePackType::ArrayTexture1D:
			return GL_TEXTURE_1D_ARRAY;

		case TexturePackType::ArrayTexture2D:
			return GL_TEXTURE_2D_ARRAY;

		case TexturePackType::CubemapTexturePositiveX:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_X;

		case TexturePackType::CubemapTextureNegativeX:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;

		case TexturePackType::CubemapTexturePositiveY:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;

		case TexturePackType::CubemapTextureNegativeY:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;

		case TexturePackType::CubemapTexturePositiveZ:
			return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;

		case TexturePackType::CubemapTextureNegativeZ:
			return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

		case TexturePackType::CubemapArrayTexture:
			return GL_TEXTURE_CUBE_MAP_ARRAY;

		default:
			gle_force_assert("Invalid texture pack type 0x%X", type);
			return -1;

	}
}


GLE_END