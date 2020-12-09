#pragma once

#include "texture.h"
#include "../gc.h"

GLE_BEGIN


class ArrayTexture1D : public Texture {

public:

	constexpr ArrayTexture1D() : Texture(TextureType::ArrayTexture1D) {}

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