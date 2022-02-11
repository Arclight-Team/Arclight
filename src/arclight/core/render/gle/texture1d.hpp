/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 texture1d.hpp
 */

#pragma once

#include "texture.hpp"
#include "glecore.hpp"

GLE_BEGIN


class Texture1D : public Texture {

public:

	constexpr Texture1D() : Texture(TextureType::Texture1D) {}

	inline void bindImageUnit(u32 unit, Access access, u32 level = 0) {
		Texture::bindImageUnit(unit, false, 0, access, level);
	}

	void setData(u32 w, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data);
	void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data);
	void update(u32 x, u32 w, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level = 0);
	
	void setCompressedData(u32 w, CompressedImageFormat format, const void* data, u32 size);
	void setCompressedMipmapData(u32 level, const void* data, u32 size);

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