/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 multisamplearraytexture2d.hpp
 */

#pragma once

#include "texture.hpp"
#include "glecore.hpp"

GLE_BEGIN


class MultisampleArrayTexture2D : public Texture {

public:

	constexpr MultisampleArrayTexture2D() : Texture(TextureType::MultisampleArrayTexture2D) {}

	void init(u32 w, u32 h, u32 layers, u32 samples, ImageFormat format, bool fixedSamples = false);

	inline void bindImageUnit(u32 unit, Access access, u32 level = 0) {
		Texture::bindImageUnit(unit, false, 0, access, level);
	}

	inline void bindImageUnit(u32 unit, u32 layer, Access access, u32 level = 0) {

		if (layer > depth) {
			error("Texture layer %d exceeds the amount of %d layers", layer, depth);
			return;
		}

		Texture::bindImageUnit(unit, true, layer, access, level);
	}

};


GLE_END