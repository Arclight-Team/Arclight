#pragma once

#include "texture.h"
#include "../gc.h"

GLE_BEGIN


class Texture1D : public Texture {

public:

	constexpr Texture1D() : Texture(TextureType::Texture1D) {}

	inline void setData(u32 w, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {
		Texture::setData(w, format, srcFormat, srcType, data);
	}

	inline void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {
		Texture::setMipmapData(level, srcFormat, srcType, data);
	}

	inline void update(u32 x, u32 w, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0) {
		Texture::update(x, w, srcFormat, srcType, data, level);
	}

	using Texture::setWrapU;
	using Texture::setMipmapBaseLevel;
	using Texture::setMipmapMaxLevel;
	using Texture::setMipmapRange;
	using Texture::setAnisotropy;
	using Texture::setMinFilter;
	using Texture::setMagFilter;
	using Texture::generateMipmaps;

};


GLE_END