#pragma once

#include "texture.h"
#include "gc.h"

GLE_BEGIN


class ArrayTexture1D : public Texture {

public:

	constexpr ArrayTexture1D() : Texture(TextureType::ArrayTexture1D) {}

	inline void update(u32 x, u32 w, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0) {
		update(x, w, 0, height, srcFormat, srcType, data, level);
	}

	inline void update(u32 x, u32 w, u32 layer, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0) {
		update(x, w, layer, 1, srcFormat, srcType, data, level);
	}

	void setData(u32 w, u32 layers, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void update(u32 x, u32 w, u32 layerStart, u32 layerCount, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0);

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