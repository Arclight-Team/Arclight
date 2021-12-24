#pragma once

#include "texture.hpp"
#include "gc.hpp"

GLE_BEGIN


class MultisampleArrayTexture2D : public Texture {

public:

	constexpr MultisampleArrayTexture2D() : Texture(TextureType::MultisampleArrayTexture2D) {}

	void init(u32 w, u32 h, u32 layers, u32 samples, ImageFormat format, bool fixedSamples = false);

};


GLE_END