#pragma once

#include "texture.h"
#include "../gc.h"

GLE_BEGIN


class CubemapArrayTexture : public Texture {

public:

	inline CubemapArrayTexture() : Texture(TextureType::CubemapArrayTexture) {
		gle_force_assert("Cubemap array textures are not supported");
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