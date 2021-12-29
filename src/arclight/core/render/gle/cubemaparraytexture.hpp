/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cubemaparraytexture.hpp
 */

#pragma once

#include "texture.hpp"
#include "gc.hpp"

GLE_BEGIN


class CubemapArrayTexture : public Texture {

public:

	inline CubemapArrayTexture() : Texture(TextureType::CubemapArrayTexture) {
		gle_force_assert("Cubemap array textures are not supported");
	}

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
	using Texture::enableComparisonMode;
	using Texture::disableComparisonMode;

};


GLE_END