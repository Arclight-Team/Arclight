#pragma once

#include "texture.h"
#include "../gc.h"

GLE_BEGIN


class Texture3D : public Texture {

public:

	constexpr Texture3D() : Texture(TextureType::Texture3D) {}

	inline void setData(u32 w, u32 h, u32 d, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {
		Texture::setData(w, h, d, format, srcFormat, srcType, data);
	}

	inline void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {
		Texture::setMipmapData(level, srcFormat, srcType, data);
	}

	inline void update(u32 x, u32 y, u32 z, u32 w, u32 h, u32 d, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0) {
		Texture::update(x, y, z, w, h, d, srcFormat, srcType, data, level);
	}

	using Texture::setWrapU;
	using Texture::setWrapV;
	using Texture::setWrapW;
	using Texture::setMipmapBaseLevel;
	using Texture::setMipmapMaxLevel;
	using Texture::setMipmapRange;
	using Texture::setAnisotropy;
	using Texture::setMinFilter;
	using Texture::setMagFilter;
	using Texture::generateMipmaps;

};


GLE_END