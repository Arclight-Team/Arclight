/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 texture2d.cpp
 */

#include "texture2d.hpp"

#include GLE_HEADER


GLE_BEGIN


void Texture2D::setData(u32 w, u32 h, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (w > Limits::getMaxTextureSize() || h > Limits::getMaxTextureSize()) {
		error("2D texture dimension of size %d exceeds maximum texture size of %d", (w > h ? w : h), Limits::getMaxTextureSize());
		return;
	}

	width = w;
	height = h;
	depth = 0;
	texFormat = format;

	glTexImage2D(glTextureTypeEnum[u32(type)], 0, static_cast<u32>(texFormat), w, h, 0, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}



void Texture2D::setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexImage2D(glTextureTypeEnum[u32(type)], level, static_cast<u32>(texFormat), getMipmapSize(level, width), getMipmapSize(level, height), 0, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}



void Texture2D::update(u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level) {

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

	glTexSubImage2D(glTextureTypeEnum[u32(type)], level, x, y, w, h, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}



void Texture2D::setCompressedData(u32 w, u32 h, CompressedImageFormat format, const void* data, u32 size) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (w > Limits::getMaxTextureSize() || h > Limits::getMaxTextureSize()) {
		error("2D texture dimension of size %d exceeds maximum texture size of %d", (w > h ? w : h), Limits::getMaxTextureSize());
		return;
	}

	width = w;
	height = h;
	depth = 0;
	texFormat = static_cast<ImageFormat>(format);

	glCompressedTexImage2D(glTextureTypeEnum[u32(type)], 0, static_cast<u32>(format), w, h, 0, size, data);

}



void Texture2D::setCompressedMipmapData(u32 level, const void* data, u32 size) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	auto format = getCompressedImageFormat();

	glCompressedTexImage2D(glTextureTypeEnum[u32(type)], level, static_cast<u32>(format), getMipmapSize(level, width), getMipmapSize(level, height), 0, size, data);

}


GLE_END