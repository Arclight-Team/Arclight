#pragma once

#include "gc.h"
#include "globject.h"
#include "imageformat.h"

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
	MultisampleArrayTexture2D
};


enum class TextureFilter {
	None,
	Bilinear,
	Trilinear
};


enum class TextureWrap {
	Repeat,
	Clamp,
	Mirror,
	Border
};


enum class CubemapFace {
	PositiveX,
	NegativeX,
	PositiveY,
	NegativeY,
	PositiveZ,
	NegativeZ
};


enum class TextureSourceFormat {
	Red,
	Green,
	Blue,
	RG,
	RGB,
	BGR,
	RGBA,
	BGRA,
	Ri,
	Gi,
	Bi,
	RGi,
	RGBi,
	BGRi,
	RGBAi,
	BGRAi,
	Depth,
	Stencil,
	DepthStencil
};


enum class TextureSourceType {
	UByte,
	Byte,
	UShort,
	Short,
	UInt,
	Int,
	Float,
	UByte332,
	UByte233,
	UShort565,
	UShort565R,
	UShort4444,
	UShort4444R,
	UShort5551,
	UShort1555,
	UInt8888,
	UInt8888R,
	UInt10_2,
	UInt2_10
};


enum class TextureOperator {
	Never,
	Always,
	Less,
	LessEqual,
	Equal,
	NotEqual,
	GreaterEqual,
	Greater
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

	static u32 getTextureTypeEnum(TextureType type);
	static u32 getTextureWrapEnum(TextureWrap wrap);
	static u32 getTextureSourceFormatEnum(TextureSourceFormat format);
	static u32 getTextureSourceTypeEnum(TextureSourceType type);
	static u32 getCubemapFaceEnum(CubemapFace face);
	static u32 getTextureOperatorEnum(TextureOperator op);

	u32 width;
	u32 height;
	u32 depth;
	ImageFormat texFormat;
	const TextureType type;

private:

	inline void setBoundTextureID(TextureType type, u32 id) const {
		boundTextureIDs[static_cast<u32>(type)] = id;
	}

	inline u32 getBoundTextureID(TextureType type) const {
		return boundTextureIDs[static_cast<u32>(type)];
	}

	static inline u32 boundTextureIDs[9] = {
		invalidBoundID, invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID, invalidBoundID 
	};

	static inline u32 activeTextureUnit = 0;

};


GLE_END