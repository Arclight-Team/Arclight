/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arraytexture2d.cpp
 */

#include "arraytexture2d.hpp"

#include "glecore.hpp"
#include GLE_HEADER


GLE_BEGIN


void ArrayTexture2D::setData(u32 w, u32 h, u32 layers, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (w > Limits::getMaxTextureSize() || h > Limits::getMaxTextureSize()) {
		error("2D array texture dimension of size %d exceeds maximum texture size of %d", (w > h ? w : h), Limits::getMaxTextureSize());
		return;
	}

	if (layers > Limits::getMaxArrayTextureLayers()) {
		error("2D array texture layer count of %d exceeds maximum array layer count of %d", layers, Limits::getMaxArrayTextureLayers());
		return;
	}

	width = w;
	height = h;
	depth = layers;
	texFormat = format;

	glTexImage3D(glTextureTypeEnum[u32(type)], 0, static_cast<u32>(texFormat), w, h, layers, 0, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}



void ArrayTexture2D::setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexImage3D(glTextureTypeEnum[u32(type)], level, static_cast<u32>(texFormat), getMipmapSize(level, width), getMipmapSize(level, height), depth, 0, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}



void ArrayTexture2D::update(u32 x, u32 y, u32 w, u32 h, u32 layerStart, u32 layerCount, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to update data)", id);

	if ((x + w) > getMipmapSize(level, width)) {
		error("Updating 2D array texture out of bounds: width = %d, requested: x = %d, w = %d", getMipmapSize(level, width), x, w);
		return;
	}

	if ((y + h) > getMipmapSize(level, height)) {
		error("Updating 2D array texture out of bounds: height = %d, requested: y = %d, h = %d", getMipmapSize(level, height), y, h);
		return;
	}

	if ((layerStart + layerCount) > depth) {
		error("Updating 2D array texture out of bounds: array size = %d, requested: start = %d, count = %d", height, layerStart, layerCount);
		return;
	}

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexSubImage3D(glTextureTypeEnum[u32(type)], level, x, y, layerStart, w, h, layerCount, static_cast<u32>(srcFormat), static_cast<u32>(srcType), data);

}


GLE_END