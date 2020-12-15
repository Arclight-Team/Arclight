#pragma once

#include "texture.h"
#include "gc.h"

GLE_BEGIN


class ArrayTexture2D : public Texture {

public:

	constexpr ArrayTexture2D() : Texture(TextureType::ArrayTexture2D) {}

	inline void update(u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0) {
		update(x, y, w, h, 0, depth, srcFormat, srcType, data, level);
	}

	inline void update(u32 x, u32 y, u32 w, u32 h, u32 layer, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0) {
		update(x, y, w, h, layer, 1, srcFormat, srcType, data, level);
	}

	void setData(u32 w, u32 h, u32 layers, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void update(u32 x, u32 y, u32 w, u32 h, u32 layerStart, u32 layerCount, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0);

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