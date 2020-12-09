#pragma once

#include "texture.h"
#include "../gc.h"

GLE_BEGIN


class Texture2D : public Texture {

public:

	constexpr Texture2D() : Texture(TextureType::Texture2D) {}

	void setData(u32 w, u32 h, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void update(u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0);

	using Texture::setWrapU;
	using Texture::setWrapV;
	using Texture::setMipmapBaseLevel;
	using Texture::setMipmapMaxLevel;
	using Texture::setMipmapRange;
	using Texture::setAnisotropy;
	using Texture::setMinFilter;
	using Texture::setMagFilter;
	using Texture::generateMipmaps;

};


GLE_END