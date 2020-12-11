#pragma once

#include "texture.h"
#include "../gc.h"

GLE_BEGIN


class MultisampleTexture2D : public Texture {

public:

	constexpr MultisampleTexture2D() : Texture(TextureType::MultisampleTexture2D) {}

	void init(u32 w, u32 h, u32 samples, TextureFormat format, bool fixedSamples = false);

};


GLE_END