/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 imageformat.cpp
 */

#include "imageformat.hpp"

#include "glecore.hpp"
#include GLE_HEADER


GLE_BEGIN


u32 Image::getMaxSamples(ImageFormat format) {

	switch (format) {

		case ImageFormat::None:
			gle_force_assert("Image format None has no sample count specified", format);
			return Limits::getMaxColorSamples();

		case ImageFormat::Depth16:
		case ImageFormat::Depth24:
		case ImageFormat::Depth32:
		case ImageFormat::Depth32f:
		case ImageFormat::Depth24Stencil8:
		case ImageFormat::Depth32fStencil8:
		case ImageFormat::Stencil8:
			return Limits::getMaxDepthSamples();

		case ImageFormat::R8i:
		case ImageFormat::R8ui:
		case ImageFormat::R16i:
		case ImageFormat::R16ui:
		case ImageFormat::R32i:
		case ImageFormat::R32ui:
		case ImageFormat::RG8i:
		case ImageFormat::RG8ui:
		case ImageFormat::RG16i:
		case ImageFormat::RG16ui:
		case ImageFormat::RG32i:
		case ImageFormat::RG32ui:
		case ImageFormat::RGB8i:
		case ImageFormat::RGB8ui:
		case ImageFormat::RGB16i:
		case ImageFormat::RGB16ui:
		case ImageFormat::RGB32i:
		case ImageFormat::RGB32ui:
		case ImageFormat::RGBA8i:
		case ImageFormat::RGBA8ui:
		case ImageFormat::RGBA16i:
		case ImageFormat::RGBA16ui:
		case ImageFormat::RGBA32i:
		case ImageFormat::RGBA32ui:
		case ImageFormat::RGB10A2ui:
			return Limits::getMaxIntegerSamples();

		default:
			return Limits::getMaxColorSamples();

	}

}



bool Image::isImageUnitCompatible(ImageFormat format) {

	switch (format) {

		case ImageFormat::R8:
		case ImageFormat::R8sn:
		case ImageFormat::R16:
		case ImageFormat::R16sn:
		case ImageFormat::RG8:
		case ImageFormat::RG8sn:
		case ImageFormat::RG16:
		case ImageFormat::RG16sn:
		case ImageFormat::RGBA8:
		case ImageFormat::RGBA8sn:
		case ImageFormat::RGB10A2:
		case ImageFormat::RGB10A2ui:
		case ImageFormat::RGBA16:
		case ImageFormat::R16f:
		case ImageFormat::RG16f:
		case ImageFormat::RGBA16f:
		case ImageFormat::R32f:
		case ImageFormat::RG32f:
		case ImageFormat::RGBA32f:
		case ImageFormat::R11fG11fB10f:
		case ImageFormat::R8i:
		case ImageFormat::R8ui:
		case ImageFormat::R16i:
		case ImageFormat::R16ui:
		case ImageFormat::R32i:
		case ImageFormat::R32ui:
		case ImageFormat::RG8i:
		case ImageFormat::RG8ui:
		case ImageFormat::RG16i:
		case ImageFormat::RG16ui:
		case ImageFormat::RG32i:
		case ImageFormat::RG32ui:
		case ImageFormat::RGBA8i:
		case ImageFormat::RGBA8ui:
		case ImageFormat::RGBA16i:
		case ImageFormat::RGBA16ui:
		case ImageFormat::RGBA32i:
		case ImageFormat::RGBA32ui:
			return true;

		default:
			return false;

	}

}



bool Image::isImageUnitCompatible(TextureBufferFormat format) {

	switch (format) {

		case TextureBufferFormat::R8:
		case TextureBufferFormat::R16:
		case TextureBufferFormat::RG8:
		case TextureBufferFormat::RG16:
		case TextureBufferFormat::RGBA8:
		case TextureBufferFormat::RGBA16:
		case TextureBufferFormat::R16f:
		case TextureBufferFormat::RG16f:
		case TextureBufferFormat::RGBA16f:
		case TextureBufferFormat::R32f:
		case TextureBufferFormat::RG32f:
		case TextureBufferFormat::RGBA32f:
		case TextureBufferFormat::R8i:
		case TextureBufferFormat::R8ui:
		case TextureBufferFormat::R16i:
		case TextureBufferFormat::R16ui:
		case TextureBufferFormat::R32i:
		case TextureBufferFormat::R32ui:
		case TextureBufferFormat::RG8i:
		case TextureBufferFormat::RG8ui:
		case TextureBufferFormat::RG16ui:
		case TextureBufferFormat::RG32i:
		case TextureBufferFormat::RG32ui:
		case TextureBufferFormat::RGBA8i:
		case TextureBufferFormat::RGBA8ui:
		case TextureBufferFormat::RGBA16i:
		case TextureBufferFormat::RGBA16ui:
		case TextureBufferFormat::RGBA32i:
		case TextureBufferFormat::RGBA32ui:
			return true;

		default:
			return false;

	}

}


GLE_END