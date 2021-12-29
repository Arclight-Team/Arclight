/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 texture3d.hpp
 */

#pragma once

#include "texture.hpp"
#include "gc.hpp"

GLE_BEGIN


class Texture3D : public Texture {

public:

	constexpr Texture3D() : Texture(TextureType::Texture3D) {}

	void setData(u32 w, u32 h, u32 d, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data);
	void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data);
	void update(u32 x, u32 y, u32 z, u32 w, u32 h, u32 d, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level = 0);

	void setCompressedData(u32 w, u32 h, u32 d, CompressedImageFormat format, const void* data, u32 size);
	void setCompressedMipmapData(u32 level, const void* data, u32 size);

	using Texture::setWrapU;
	using Texture::setWrapV;
	using Texture::setWrapW;
	using Texture::setBorderColor;
	using Texture::setMipmapBaseLevel;
	using Texture::setMipmapMaxLevel;
	using Texture::setMipmapRange;
	using Texture::setAnisotropy;
	using Texture::setMinFilter;
	using Texture::setMagFilter;
	using Texture::generateMipmaps;

};


GLE_END