/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 MultisampleTexture2D.hpp
 */

#pragma once

#include "Texture.hpp"
#include "GC.hpp"

GLE_BEGIN


class MultisampleTexture2D : public Texture {

public:

	constexpr MultisampleTexture2D() : Texture(TextureType::MultisampleTexture2D) {}

	void init(u32 w, u32 h, u32 samples, ImageFormat format, bool fixedSamples = false);

	inline void bindImageUnit(u32 unit, Access access, u32 level = 0) {
		Texture::bindImageUnit(unit, false, 0, access, level);
	}

};


GLE_END