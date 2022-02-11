/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cubemaptexture.cpp
 */

#include "cubemaptexture.hpp"

#include "glecore.hpp"
#include GLE_HEADER


GLE_BEGIN


void CubemapTexture::setData(CubemapFace face, u32 s, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (s > Limits::getMaxTextureSize()) {
		error("2D cubemap texture dimension of size %d exceeds maximum texture size of %d", s, Limits::getMaxTextureSize());
		return;
	}

	if (texFormat == ImageFormat::None) {

		width = s;
		height = s;
		depth = 0;
		texFormat = format;

	} else if (texFormat != format || width != s || height != s) {
		error("Cubemap initialization inconsistent");
		return;
	}

	glTexImage2D(static_cast<u32>(face), 0, static_cast<u32>(texFormat), s, s, 0, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}



void CubemapTexture::setMipmapData(CubemapFace face, u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexImage2D(static_cast<u32>(face), level, static_cast<u32>(texFormat), getMipmapSize(level, width), getMipmapSize(level, height), 0, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}



void CubemapTexture::update(CubemapFace face, u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to update data)", id);

	if ((x + w) > getMipmapSize(level, width)) {
		error("Updating 2D texture out of bounds: width = %d, requested: x = %d, w = %d", getMipmapSize(level, width), x, w);
		return;
	}

	if ((y + h) > getMipmapSize(level, height)) {
		error("Updating 2D texture out of bounds: height = %d, requested: y = %d, h = %d", getMipmapSize(level, height), y, h);
		return;
	}

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexSubImage2D(static_cast<u32>(face), level, x, y, w, h, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}


GLE_END