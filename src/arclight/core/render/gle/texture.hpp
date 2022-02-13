/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 texture.hpp
 */

#pragma once

#include "glecore.hpp"
#include "globject.hpp"
#include "imageformat.hpp"

GLE_BEGIN


enum class TextureType {
	Texture1D,
	Texture2D,
	Texture3D,
	ArrayTexture1D,
	ArrayTexture2D,
	CubemapTexture,
	CubemapArrayTexture,
	MultisampleTexture2D,
	MultisampleArrayTexture2D,
	TextureBuffer
};


enum class TextureFilter {
	None,
	Bilinear,
	Trilinear
};


enum class TextureWrap {
	Repeat	= 0x812F, // GL_CLAMP_TO_EDGE
	Clamp	= 0x8370, // GL_MIRRORED_REPEAT
	Mirror	= 0x2901, // GL_REPEAT
	Border	= 0x812D, // GL_CLAMP_TO_BORDER
};


enum class CubemapFace {
	PositiveX = 0x8515, // GL_TEXTURE_CUBE_MAP_POSITIVE_X
	NegativeX = 0x8516, // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
	PositiveY = 0x8517, // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
	NegativeY = 0x8518, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
	PositiveZ = 0x8519, // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
	NegativeZ = 0x851A, // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};


enum class TextureSourceFormat {
	Red				= 0x1903, // GL_RED
	Green			= 0x1904, // GL_GREEN
	Blue			= 0x1905, // GL_BLUE
	RG				= 0x8227, // GL_RG
	RGB				= 0x1907, // GL_RGB
	BGR				= 0x80E0, // GL_BGR
	RGBA			= 0x1908, // GL_RGBA
	BGRA			= 0x80E1, // GL_BGRA
	Ri				= 0x8D94, // GL_RED_INTEGER
	Gi				= 0x8D95, // GL_GREEN_INTEGER
	Bi				= 0x8D96, // GL_BLUE_INTEGER
	RGi				= 0x8228, // GL_RG_INTEGER
	RGBi			= 0x8D98, // GL_RGB_INTEGER
	BGRi			= 0x8D9A, // GL_BGR_INTEGER
	RGBAi			= 0x8D99, // GL_RGBA_INTEGER
	BGRAi			= 0x8D9B, // GL_BGRA_INTEGER
	Depth			= 0x1902, // GL_DEPTH_COMPONENT
	Stencil			= 0x1901, // GL_STENCIL_INDEX
	DepthStencil	= 0x84F9, // GL_DEPTH_STENCI
};


enum class TextureSourceType {
	UByte		= 0x1401, // GL_UNSIGNED_BYTE
	Byte		= 0x1400, // GL_BYTE
	UShort		= 0x1403, // GL_UNSIGNED_SHORT
	Short		= 0x1402, // GL_SHORT
	UInt		= 0x1405, // GL_UNSIGNED_INT
	Int			= 0x1404, // GL_INT
	Float		= 0x1406, // GL_FLOAT
	UByte332	= 0x8032, // GL_UNSIGNED_BYTE_3_3_2
	UByte233	= 0x8362, // GL_UNSIGNED_BYTE_2_3_3_REV
	UShort565	= 0x8363, // GL_UNSIGNED_SHORT_5_6_5
	UShort565R	= 0x8364, // GL_UNSIGNED_SHORT_5_6_5_REV
	UShort4444	= 0x8033, // GL_UNSIGNED_SHORT_4_4_4_4
	UShort4444R	= 0x8365, // GL_UNSIGNED_SHORT_4_4_4_4_REV
	UShort5551	= 0x8034, // GL_UNSIGNED_SHORT_5_5_5_1
	UShort1555	= 0x8366, // GL_UNSIGNED_SHORT_1_5_5_5_REV
	UInt8888	= 0x8035, // GL_UNSIGNED_INT_8_8_8_8
	UInt8888R	= 0x8367, // GL_UNSIGNED_INT_8_8_8_8_REV
	UInt10_2	= 0x8036, // GL_UNSIGNED_INT_10_10_10_2
	UInt2_10	= 0x8368, // GL_UNSIGNED_INT_2_10_10_10_REV
};


enum class TextureOperator {
	Never			= 0x0200, // GL_NEVER
	Always			= 0x0207, // GL_ALWAYS
	Less			= 0x0201, // GL_LESS
	LessEqual		= 0x0203, // GL_LEQUAL
	Equal			= 0x0202, // GL_EQUAL
	NotEqual		= 0x0205, // GL_NOTEQUAL
	GreaterEqual	= 0x0206, // GL_GEQUAL
	Greater			= 0x0204, // GL_GREATER
};



class Texture : public GLObject {

public:

	//Default mipmap levels
	constexpr static u32 defaultBaseLevel = 0;
	constexpr static u32 defaultMaxLevel = 1000;

	//Creates a texture if none has been created yet
	virtual bool create() override;

	//Binds the texture if not already. Fails if it hasn't been created yet.
	void bind();

	//Unbinds the texture. Fails if it hasn't been bound.
	void unbind();

	//Destroys a texture if it was created once
	virtual void destroy() override;

	void activate(u32 unit);
	static void activateUnit(u32 unit);

	//Checks the given states
	bool isBound() const;
	bool isInitialized() const;

	u32 getMaxDimension() const;
	u32 getMipmapCount() const;

	u32 getWidth() const;
	u32 getHeight() const;
	u32 getDepth() const;
	ImageFormat getImageFormat() const;
	CompressedImageFormat getCompressedImageFormat() const;

	TextureType getTextureType() const;

	static u32 getMipmapSize(u32 level, u32 d);
	static CubemapFace getCubemapFace(u32 index);

protected:

	friend class Framebuffer;
	friend class PixelPackBuffer;

	//Don't even try creating a raw texture object.
	constexpr explicit Texture(TextureType type) : type(type),
		width(0), height(0), depth(0), texFormat(ImageFormat::None) {}

	void setWrapU(TextureWrap wrap);
	void setWrapV(TextureWrap wrap);
	void setWrapW(TextureWrap wrap);

	void setBorderColor(float r, float g, float b, float a);

	void setMipmapBaseLevel(u32 level);
	void setMipmapMaxLevel(u32 level);
	void setMipmapRange(u32 base, u32 max);

	void setAnisotropy(float a);

	void setMinFilter(TextureFilter filter, bool mipmapped = true);
	void setMagFilter(TextureFilter filter);

	void enableComparisonMode(TextureOperator op = TextureOperator::Less);
	void disableComparisonMode();

	void generateMipmaps();

	void bindImageUnit(u32 unit, bool layered, u32 layer, Access access, u32 level);

	constexpr static u32 glTextureTypeEnum[] = {
		0x0DE0, // GL_TEXTURE_1D
		0x0DE1, // GL_TEXTURE_2D
		0x806F, // GL_TEXTURE_3D
		0x8C18, // GL_TEXTURE_1D_ARRAY
		0x8C1A, // GL_TEXTURE_2D_ARRAY
		0x8513, // GL_TEXTURE_CUBE_MAP
		0x9009, // GL_TEXTURE_CUBE_MAP_ARRAY
		0x9100, // GL_TEXTURE_2D_MULTISAMPLE
		0x9102, // GL_TEXTURE_2D_MULTISAMPLE_ARRAY
		0x8C2A, // GL_TEXTURE_BUFFER
	};

	u32 width;
	u32 height;
	u32 depth;
	ImageFormat texFormat;
	const TextureType type;

private:

	static inline void setBoundTextureID(TextureType type, u32 id) {
		boundTextureIDs[u32(type)] = id;
	}

	static inline u32 getBoundTextureID(TextureType type) {
		return boundTextureIDs[u32(type)];
	}

	static inline u32 boundTextureIDs[10] = {
		invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID
	};

	static inline u32 activeTextureUnit = 0;

};


GLE_END