/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arraytexture1d.hpp
 */

#pragma once

#include "texture.hpp"
#include "glecore.hpp"

GLE_BEGIN


class ArrayTexture1D : public Texture {

public:

	constexpr ArrayTexture1D() : Texture(TextureType::ArrayTexture1D) {}

	inline void update(u32 x, u32 w, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level = 0) {
		update(x, w, 0, height, srcFormat, srcType, data, level);
	}

	inline void update(u32 x, u32 w, u32 layer, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level = 0) {
		update(x, w, layer, 1, srcFormat, srcType, data, level);
	}

	inline void bindImageUnit(u32 unit, Access access, u32 level = 0) {
		Texture::bindImageUnit(unit, false, 0, access, level);
	}

	inline void bindImageUnit(u32 unit, u32 layer, Access access, u32 level = 0) {

		if (layer > height) {
			error("Texture layer %d exceeds the amount of %d layers", layer, height);
			return;
		}

		Texture::bindImageUnit(unit, true, layer, access, level);
	}

	void setData(u32 w, u32 layers, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data);
	void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data);
	void update(u32 x, u32 w, u32 layerStart, u32 layerCount, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level = 0);

	using Texture::setWrapU;
	using Texture::setBorderColor;
	using Texture::setMipmapBaseLevel;
	using Texture::setMipmapMaxLevel;
	using Texture::setMipmapRange;
	using Texture::setAnisotropy;
	using Texture::setMinFilter;
	using Texture::setMagFilter;
	using Texture::generateMipmaps;
	using Texture::enableComparisonMode;
	using Texture::disableComparisonMode;

};


GLE_END