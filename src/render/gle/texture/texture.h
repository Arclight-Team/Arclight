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
	CubeMapTexture,
	CubeMapArrayTexture,
	MultisampleTexture2D,
	MultisampleTextureArray2D
};



enum class TextureFilter {
	None,
	Bilinear,
	Trilinear
};



class Texture {

public:

	//Creates a texture if none has been created yet
	void create();

	//Binds the texture if not already. Fails if it hasn't been created yet.
	void bind();

	//Destroys a texture if it was created once
	void destroy();

	//Checks the given states
	bool isCreated() const;
	bool isBound() const;

	u32 getMipmapCount() const;

	void setMipmapBaseLevel(u32 level);
	void setMipmapMaxLevel(u32 level);
	void setMipmapRange(u32 base, u32 max);

	static void enableAutomaticMipmapGeneration();
	static void disableAutomaticMipmapGeneration();

protected:

	//Don't even try creating a raw texture object.
	constexpr explicit Texture(TextureType type) : id(invalidID), type(type), format(TextureFormat::RGBA8) {}

	//And no copy-construction.
	Texture(const Texture& texture) = delete;
	Texture& operator=(const Texture& texture) = delete;

	void setData(u32 w, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 mipmapLevel = 0);
	void setData(u32 w, u32 h, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 mipmapLevel = 0);
	void setData(u32 w, u32 h, u32 d, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 mipmapLevel = 0);

	void update(u32 x, u32 w, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 mipmapLevel = 0);
	void update(u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 mipmapLevel = 0);
	void update(u32 x, u32 y, u32 z, u32 w, u32 h, u32 d, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 mipmapLevel = 0);

	void generateMipmaps();

private:

	constexpr void setBoundTextureID(TextureType type, u32 id) const {
		boundTextureIDs[static_cast<u32>(type)] = id;
	}

	constexpr u32 getBoundTextureID(TextureType type) const {
		return boundTextureIDs[static_cast<u32>(type)];
	}

	static u32 getTextureTypeEnum(TextureType type);
	static u32 getTextureFormatEnum(TextureFormat format);
	static u32 getCompressedTextureFormatEnum(CompressedTextureFormat format);
	static u32 getTextureSourceFormatEnum(TextureSourceFormat format);
	static u32 getTextureSourceTypeEnum(TextureSourceType type);

	u32 id;
	TextureFormat format;
	const TextureType type;

	static inline bool autoGenMipmaps = false;
	static inline u32 boundTextureIDs[9] = {
		invalidBoundID, invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID, invalidBoundID,
		invalidBoundID, invalidBoundID, invalidBoundID 
	};

};


GLE_END