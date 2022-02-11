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


u32 Image::getImageFormatEnum(ImageFormat format) {

	switch (format) {

		case ImageFormat::R8:
			return GL_R8;

		case ImageFormat::R8sn:
			return GL_R8_SNORM;

		case ImageFormat::R16:
			return GL_R16;

		case ImageFormat::R16sn:
			return GL_R16_SNORM;

		case ImageFormat::RG8:
			return GL_RG8;

		case ImageFormat::RG8sn:
			return GL_RG8_SNORM;

		case ImageFormat::RG16:
			return GL_RG16;

		case ImageFormat::RG16sn:
			return GL_R16_SNORM;

		case ImageFormat::R3G3B2:
			return GL_R3_G3_B2;

		case ImageFormat::RGB4:
			return GL_RGB4;

		case ImageFormat::RGB5:
			return GL_RGB5;

		case ImageFormat::RGB8:
			return GL_RGB8;

		case ImageFormat::RGB8sn:
			return GL_RGB8_SNORM;

		case ImageFormat::RGB10:
			return GL_RGB10;

		case ImageFormat::RGB12:
			return GL_RGB12;

		case ImageFormat::RGB16sn:
			return GL_RGB16_SNORM;

		case ImageFormat::RGBA2:
			return GL_RGBA2;

		case ImageFormat::RGBA4:
			return GL_RGBA4;

		case ImageFormat::RGB5A1:
			return GL_RGB5_A1;

		case ImageFormat::RGBA8:
			return GL_RGBA8;

		case ImageFormat::RGBA8sn:
			return GL_RGBA8_SNORM;

		case ImageFormat::RGB10A2:
			return GL_RGB10_A2;

		case ImageFormat::RGB10A2ui:
			return GL_RGB10_A2UI;

		case ImageFormat::RGBA12:
			return GL_RGBA12;

		case ImageFormat::RGBA16:
			return GL_RGBA16;

		case ImageFormat::SRGB8:
			return GL_SRGB8;

		case ImageFormat::SRGBA8:
			return GL_SRGB8_ALPHA8;

		case ImageFormat::R16f:
			return GL_R16F;

		case ImageFormat::RG16f:
			return GL_RG16F;

		case ImageFormat::RGB16f:
			return GL_RGB16F;

		case ImageFormat::RGBA16f:
			return GL_RGBA16F;

		case ImageFormat::R32f:
			return GL_R32F;

		case ImageFormat::RG32f:
			return GL_RG32F;

		case ImageFormat::RGB32f:
			return GL_RGB32F;

		case ImageFormat::RGBA32f:
			return GL_RGBA32F;

		case ImageFormat::R11fG11fB10f:
			return GL_R11F_G11F_B10F;

		case ImageFormat::RGB9E5:
			return GL_RGB9_E5;

		case ImageFormat::R8i:
			return GL_R8I;

		case ImageFormat::R8ui:
			return GL_R8UI;

		case ImageFormat::R16i:
			return GL_R16I;

		case ImageFormat::R16ui:
			return GL_R16UI;

		case ImageFormat::R32i:
			return GL_R32I;

		case ImageFormat::R32ui:
			return GL_R32UI;

		case ImageFormat::RG8i:
			return GL_RG8I;

		case ImageFormat::RG8ui:
			return GL_RG8UI;

		case ImageFormat::RG16i:
			return GL_RG16I;

		case ImageFormat::RG16ui:
			return GL_RG16UI;

		case ImageFormat::RG32i:
			return GL_RG32I;

		case ImageFormat::RG32ui:
			return GL_RG32UI;

		case ImageFormat::RGB8i:
			return GL_RGB8I;

		case ImageFormat::RGB8ui:
			return GL_RGB8UI;

		case ImageFormat::RGB16i:
			return GL_RGB16I;

		case ImageFormat::RGB16ui:
			return GL_RGB16UI;

		case ImageFormat::RGB32i:
			return GL_RGB32I;

		case ImageFormat::RGB32ui:
			return GL_RGB32UI;

		case ImageFormat::RGBA8i:
			return GL_RGBA8I;

		case ImageFormat::RGBA8ui:
			return GL_RGBA8UI;

		case ImageFormat::RGBA16i:
			return GL_RGBA16I;

		case ImageFormat::RGBA16ui:
			return GL_RGBA16UI;

		case ImageFormat::RGBA32i:
			return GL_RGBA32I;

		case ImageFormat::RGBA32ui:
			return GL_RGBA32UI;

		case ImageFormat::Depth16:
			return GL_DEPTH_COMPONENT16;

		case ImageFormat::Depth24:
			return GL_DEPTH_COMPONENT24;

		case ImageFormat::Depth32:
			return GL_DEPTH_COMPONENT32;

		case ImageFormat::Depth32f:
			return GL_DEPTH_COMPONENT32F;

		case ImageFormat::Depth24Stencil8:
			return GL_DEPTH24_STENCIL8;

		case ImageFormat::Depth32fStencil8:
			return GL_DEPTH32F_STENCIL8;

		case ImageFormat::Stencil8:
			return GL_STENCIL_INDEX8;

		default:
			gle_force_assert("Invalid texture format 0x%X", format);
			return -1;

	}

}



u32 Image::getCompressedImageFormatEnum(CompressedImageFormat format) {

	switch (format) {

		case CompressedImageFormat::RGTC1:
			return GL_COMPRESSED_RED_RGTC1;

		case CompressedImageFormat::RGTC1s:
			return GL_COMPRESSED_SIGNED_RED_RGTC1;

		case CompressedImageFormat::RGTC2:
			return GL_COMPRESSED_RG_RGTC2;

		case CompressedImageFormat::RGTC2s:
			return GL_COMPRESSED_SIGNED_RG_RGTC2;

		case CompressedImageFormat::RGBA_BPTCun:
			return GL_COMPRESSED_RGBA_BPTC_UNORM;

		case CompressedImageFormat::SRGB_BPTCun:
			return GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;

		case CompressedImageFormat::BPTCf:
			return GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT;

		case CompressedImageFormat::BPTCuf:
			return GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT;

		case CompressedImageFormat::RGB_DXT1:
			return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;

		case CompressedImageFormat::SRGB_DXT1:
			return GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;

		case CompressedImageFormat::RGBA_DXT1:
			return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;

		case CompressedImageFormat::SRGBA_DXT1:
			return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;

		case CompressedImageFormat::RGBA_DXT3:
			return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;

		case CompressedImageFormat::SRGBA_DXT3:
			return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;

		case CompressedImageFormat::RGBA_DXT5:
			return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

		case CompressedImageFormat::SRGBA_DXT5:
			return GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;

		default:
			gle_force_assert("Invalid compressed texture format 0x%X", format);
			return -1;

	}

}



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



u32 Image::getImageFormatEnum(TextureBufferFormat format) {

	switch (format) {

		case TextureBufferFormat::R8:
			return GL_R8;

		case TextureBufferFormat::R16:
			return GL_R16;

		case TextureBufferFormat::RG8:
			return GL_RG8;

		case TextureBufferFormat::RG16:
			return GL_RG16;

		case TextureBufferFormat::RGBA16:
			return GL_RGBA16;

		case TextureBufferFormat::R16f:
			return GL_R16F;

		case TextureBufferFormat::RG16f:
			return GL_RG16F;

		case TextureBufferFormat::RGBA16f:
			return GL_RGBA16F;

		case TextureBufferFormat::R32f:
			return GL_R32F;

		case TextureBufferFormat::RG32f:
			return GL_RG32F;

		case TextureBufferFormat::RGB32f:
			return GL_RGB32F;

		case TextureBufferFormat::RGBA32f:
			return GL_RGBA32F;

		case TextureBufferFormat::R8i:
			return GL_R8I;

		case TextureBufferFormat::R8ui:
			return GL_R8UI;

		case TextureBufferFormat::R16i:
			return GL_R16I;

		case TextureBufferFormat::R16ui:
			return GL_R16UI;

		case TextureBufferFormat::R32i:
			return GL_R32I;

		case TextureBufferFormat::R32ui:
			return GL_R32UI;

		case TextureBufferFormat::RG8i:
			return GL_RG8I;

		case TextureBufferFormat::RG8ui:
			return GL_RG8UI;

		case TextureBufferFormat::RG16ui:
			return GL_RG16UI;

		case TextureBufferFormat::RG32i:
			return GL_RG32I;

		case TextureBufferFormat::RG32ui:
			return GL_RG32UI;

		case TextureBufferFormat::RGB32i:
			return GL_RGB32I;

		case TextureBufferFormat::RGB32ui:
			return GL_RGB32UI;

		case TextureBufferFormat::RGBA8:
			return GL_RGBA8;

		case TextureBufferFormat::RGBA8i:
			return GL_RGBA8I;

		case TextureBufferFormat::RGBA8ui:
			return GL_RGBA8UI;

		case TextureBufferFormat::RGBA16i:
			return GL_RGBA16I;

		case TextureBufferFormat::RGBA16ui:
			return GL_RGBA16UI;

		case TextureBufferFormat::RGBA32i:
			return GL_RGBA32I;

		case TextureBufferFormat::RGBA32ui:
			return GL_RGBA32UI;

		default:
			gle_force_assert("Invalid texture buffer format 0x%X", format);
			return -1;

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