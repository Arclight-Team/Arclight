#pragma once

#include "texture.hpp"
#include "gc.hpp"

GLE_BEGIN


class MultisampleTexture2D : public Texture {

public:

	constexpr MultisampleTexture2D() : Texture(TextureType::MultisampleTexture2D) {}

	void init(u32 w, u32 h, u32 samples, ImageFormat format, bool fixedSamples = false);

};


GLE_END