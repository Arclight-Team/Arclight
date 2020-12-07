#pragma once

#include "../gc.h"
#include "textureformat.h"

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
	Mirror
};


enum class CubemapFace {
	PositiveX,
	NegativeX,
	PositiveY,
	NegativeY,
	PositiveZ,
	NegativeZ
};


class Texture {

public:

	//Default mipmap levels
	constexpr static inline u32 defaultBaseLevel = 0;
	constexpr static inline u32 defaultMaxLevel = 1000;

	//Creates a texture if none has been created yet
	void create();

	//Binds the texture if not already. Fails if it hasn't been created yet.
	void bind();

	//Destroys a texture if it was created once
	void destroy();

	void activate(u32 unit);
	static void activateUnit(u32 unit);

	//Checks the given states
	bool isCreated() const;
	bool isBound() const;

	u32 getMaxDimension() const;
	u32 getMipmapCount() const;

	bool isMultisampleTexture() const;

	static u32 getMipmapSize(u32 level, u32 d);
	static CubemapFace getCubemapFace(u32 index);

protected:

	//Don't even try creating a raw texture object.
	constexpr explicit Texture(TextureType type) : id(invalidID), type(type),
		width(0), height(0), depth(0), texFormat(TextureFormat::None) {}

	//And no copy-construction.
	Texture(const Texture& texture) = delete;
	Texture& operator=(const Texture& texture) = delete;

	void setData(u32 w, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void setData(u32 w, u32 h, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void setData(u32 w, u32 h, u32 d, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void setData(CubemapFace face, u32 s, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);

	void setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);
	void setMipmapData(CubemapFace face, u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data);

	void update(u32 x, u32 w, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0);
	void update(u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0);
	void update(u32 x, u32 y, u32 z, u32 w, u32 h, u32 d, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0);
	void update(CubemapFace face, u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level = 0);

	void setWrapU(TextureWrap wrap);
	void setWrapV(TextureWrap wrap);
	void setWrapW(TextureWrap wrap);

	void setMipmapBaseLevel(u32 level);
	void setMipmapMaxLevel(u32 level);
	void setMipmapRange(u32 base, u32 max);

	void setAnisotropy(float a);

	void setMinFilter(TextureFilter filter, bool mipmapped = true);
	void setMagFilter(TextureFilter filter);

	void generateMipmaps();

private:

	constexpr void setBoundTextureID(TextureType type, u32 id) const {
		boundTextureIDs[static_cast<u32>(type)] = id;
	}

	constexpr u32 getBoundTextureID(TextureType type) const {
		return boundTextureIDs[static_cast<u32>(type)];
	}

	static u32 getTextureTypeEnum(TextureType type);
	static u32 getTextureWrapEnum(TextureWrap wrap);
	static u32 getTextureFormatEnum(TextureFormat format);
	static u32 getCompressedTextureFormatEnum(CompressedTextureFormat format);
	static u32 getTextureSourceFormatEnum(TextureSourceFormat format);
	static u32 getTextureSourceTypeEnum(TextureSourceType type);
	static u32 getCubemapFaceEnum(CubemapFace face);

	u32 id;
	const TextureType type;

	u32 width;
	u32 height;
	u32 depth;
	TextureFormat texFormat;

	static inline u32 boundTextureIDs[9] = {
		invalidBoundID, invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID, invalidBoundID 
	};

	static inline u32 activeTextureUnit = 0;

};


GLE_END