#include "multisampletexture2d.h"

#include "glecore.h"
#include GLE_HEADER


GLE_BEGIN


void MultisampleTexture2D::init(u32 w, u32 h, u32 samples, TextureFormat format, bool fixedSamples) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to initialize)", id);

	if (w > Limits::getMaxTextureSize() || h > Limits::getMaxTextureSize()) {
		GLE::error("2D multisample texture dimension of size %d exceeds maximum texture size of %d", (w > h ? w : h), Limits::getMaxTextureSize());
		return;
	}

	if (!samples) {
		GLE::error("2d multisample texture cannot have 0 samples");
		return;
	}

	switch (format) {

		case TextureFormat::None:
			GLE::error("Format for 2D multisample array texture not specified");
			break;

		case TextureFormat::Depth16:
		case TextureFormat::Depth24:
		case TextureFormat::Depth32:
		case TextureFormat::Depth32f:
		case TextureFormat::Depth24Stencil8:
		case TextureFormat::Depth32fStencil8:
		case TextureFormat::Stencil8:

			if (samples > Limits::getMaxDepthSamples()) {
				GLE::error("2D multisample texture with depth/stencil-renderable format cannot hold more than %d samples (got %d)", Limits::getMaxDepthSamples(), samples);
				return;
			}

			break;

		case TextureFormat::R8i:
		case TextureFormat::R8ui:
		case TextureFormat::R16i:
		case TextureFormat::R16ui:
		case TextureFormat::R32i:
		case TextureFormat::R32ui:
		case TextureFormat::RG8i:
		case TextureFormat::RG8ui:
		case TextureFormat::RG16i:
		case TextureFormat::RG16ui:
		case TextureFormat::RG32i:
		case TextureFormat::RG32ui:
		case TextureFormat::RGB8i:
		case TextureFormat::RGB8ui:
		case TextureFormat::RGB16i:
		case TextureFormat::RGB16ui:
		case TextureFormat::RGB32i:
		case TextureFormat::RGB32ui:
		case TextureFormat::RGBA8i:
		case TextureFormat::RGBA8ui:
		case TextureFormat::RGBA16i:
		case TextureFormat::RGBA16ui:
		case TextureFormat::RGBA32i:
		case TextureFormat::RGBA32ui:
		case TextureFormat::RGB10A2ui:

			if (samples > Limits::getMaxIntegerSamples()) {
				GLE::error("2D multisample texture with integer format cannot hold more than %d samples (got %d)", Limits::getMaxIntegerSamples(), samples);
				return;
			}

			break;

		default:

			if (samples > Limits::getMaxColorSamples()) {
				GLE::error("2D multisample texture with color-renderable format cannot hold more than %d samples (got %d)", Limits::getMaxColorSamples(), samples);
				return;
			}

			break;

	}

	width = w;
	height = h;
	depth = 0;
	texFormat = format;

	glTexImage2DMultisample(getTextureTypeEnum(type), samples, getTextureFormatEnum(format), w, h, fixedSamples);

}


GLE_END