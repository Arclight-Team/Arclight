/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 multisampletexture2d.cpp
 */

#include "multisampletexture2d.hpp"

#include GLE_HEADER


GLE_BEGIN


void MultisampleTexture2D::init(u32 w, u32 h, u32 samples, ImageFormat format, bool fixedSamples) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to initialize)", id);

	if (w > Limits::getMaxTextureSize() || h > Limits::getMaxTextureSize()) {
		error("2D multisample texture dimension of size %d exceeds maximum texture size of %d", (w > h ? w : h), Limits::getMaxTextureSize());
		return;
	}

	if (!samples) {
		error("2d multisample texture cannot have 0 samples");
		return;
	}

	if (samples > Image::getMaxSamples(format)) {
		error("2D multisample texture with given format cannot hold more than %d samples (got %d)", Image::getMaxSamples(format), samples);
		return;
	}

	width = w;
	height = h;
	depth = 0;
	texFormat = format;

	glTexImage2DMultisample(glTextureTypeEnum[u32(type)], samples, static_cast<u32>(format), w, h, fixedSamples);

}


GLE_END