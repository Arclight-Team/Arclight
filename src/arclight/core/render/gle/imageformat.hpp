/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 imageformat.hpp
 */

#pragma once

#include "gc.hpp"

GLE_BEGIN

enum class ImageFormat {
	None,
	R8,
	R8sn,
	R16,
	R16sn,
	RG8,
	RG8sn,
	RG16,
	RG16sn,
	R3G3B2,
	RGB4,
	RGB5,
	RGB8,
	RGB8sn,
	RGB10,
	RGB12,
	RGB16sn,
	RGBA2,
	RGBA4,
	RGB5A1,
	RGBA8,
	RGBA8sn,
	RGB10A2,
	RGB10A2ui,
	RGBA12,
	RGBA16,
	SRGB8,
	SRGBA8,
	R16f,
	RG16f,
	RGB16f,
	RGBA16f,
	R32f,
	RG32f,
	RGB32f,
	RGBA32f,
	R11fG11fB10f,
	RGB9E5,
	R8i,
	R8ui,
	R16i,
	R16ui,
	R32i,
	R32ui,
	RG8i,
	RG8ui,
	RG16i,
	RG16ui,
	RG32i,
	RG32ui,
	RGB8i,
	RGB8ui,
	RGB16i,
	RGB16ui,
	RGB32i,
	RGB32ui,
	RGBA8i,
	RGBA8ui,
	RGBA16i,
	RGBA16ui,
	RGBA32i,
	RGBA32ui,
	Depth16,
	Depth24,
	Depth32,
	Depth32f,
	Depth24Stencil8,
	Depth32fStencil8,
	Stencil8
};


enum class CompressedImageFormat {
	RGTC1,
	RGTC1s,
	RGTC2,
	RGTC2s,
	RGBA_BPTCun,
	SRGB_BPTCun,
	BPTCf,
	BPTCuf,
	RGB_DXT1,
	SRGB_DXT1,
	RGBA_DXT1,
	SRGBA_DXT1,
	RGBA_DXT3,
	SRGBA_DXT3,
	RGBA_DXT5,
	SRGBA_DXT5,
};


enum class TextureBufferFormat {
	R8,
	R16,
	R16f,
	R32f,
	R8i,
	R16i,
	R32i,
	R8ui,
	R16ui,
	R32ui,
	RG8,
	RG16,
	RG16f,
	RG32f,
	RG8i,
	RG32i,
	RG8ui,
	RG16ui,
	RG32ui,
	RGB32f,		// need ARB_texture_buffer_object_rgb32
	RGB32i,		//
	RGB32ui,	//
	RGBA8,
	RGBA16,
	RGBA16f,
	RGBA32f,
	RGBA8i,
	RGBA16i,
	RGBA32i,
	RGBA8ui,
	RGBA16ui,
	RGBA32ui
};


namespace Image {

	//Returns the corresponding enum value of format
	u32 getImageFormatEnum(ImageFormat format);
	u32 getImageFormatEnum(TextureBufferFormat format);
	u32 getCompressedImageFormatEnum(CompressedImageFormat format);

	//Returns the maximum number of samples of the given format
	u32 getMaxSamples(ImageFormat format);

	//Returns true if the format can be used for image load store operations
	bool isImageUnitCompatible(ImageFormat format);
	bool isImageUnitCompatible(TextureBufferFormat format);

}


GLE_END