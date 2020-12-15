#include "texture.h"

#include "glecore.h"
#include GLE_HEADER


GLE_BEGIN


bool Texture::create() {

	if (!isCreated()) {

		glGenTextures(1, &id);

		if (!id) {

			id = invalidID;
			return false;

		}

	}

	return true;

}



void Texture::bind() {

	gle_assert(isCreated(), "Texture hasn't been created yet");

	if (!isBound()) {
		glBindTexture(getTextureTypeEnum(type), id);
		setBoundTextureID(type, id);
	}

}



void Texture::destroy() {

	if (isCreated()) {

		if (isBound()) {
			setBoundTextureID(type, invalidBoundID);
		}

		glDeleteTextures(1, &id);

		id = invalidID;
		width = 0;
		height = 0;
		depth = 0;
		texFormat = ImageFormat::None;

	}

}



void Texture::activate(u32 unit) {

	activateUnit(unit);

	//Force-bind
	glBindTexture(getTextureTypeEnum(type), id);
	setBoundTextureID(type, id);

}



void Texture::activateUnit(u32 unit) {

	gle_assert(unit < Limits::getMaxTextureUnits(), "Texture unit %d exceeds the maximum of %d", unit, Limits::getMaxTextureUnits());

	if (activeTextureUnit != unit) {
		activeTextureUnit = unit;
		glActiveTexture(GL_TEXTURE0 + unit);
	}

}



bool Texture::isBound() const {
	return id == getBoundTextureID(type);
}



bool Texture::isInitialized() const {
	return texFormat != ImageFormat::None;
}



u32 Texture::getMaxDimension() const {
	return (width > height ? (depth > width ? depth : width) : (depth > height ? depth : height));
}



u32 Texture::getMipmapCount() const {

	u32 maxDim = getMaxDimension();
	u32 count = 0;

	while (maxDim /= 2) {
		count++;
	}

	return count;

}



u32 Texture::getWidth() const {
	return width;
}



u32 Texture::getHeight() const {
	return height;
}



u32 Texture::getDepth() const {
	return depth;
}



ImageFormat Texture::getImageFormat() const {
	return texFormat;
}



u32 Texture::getMipmapSize(u32 level, u32 d) {
	return d >> level;
}



void Texture::setMipmapBaseLevel(u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set base mipmap level)", id);

	glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_BASE_LEVEL, level);

}



void Texture::setMipmapMaxLevel(u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set max mipmap level)", id);

	glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_MAX_LEVEL, level);

}



void Texture::setMipmapRange(u32 base, u32 max) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap boundary levels)", id);
	gle_assert(base <= max, "Mipmap base level cannot be greater than the max level (base = %d, max = %d)", base, max);

	setMipmapBaseLevel(base);
	setMipmapMaxLevel(max);

}



void Texture::setAnisotropy(float a) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set texture anisotropy)", id);

	float maxAnisotropy = Limits::getMaxTextureAnisotropy();

	//Anisotropy has no effect in this case (and is not supported)
	if (maxAnisotropy == 1.0f) {
		return;
	}

	if (a > maxAnisotropy) {
		warn("Specified anisotropy level of %d exceeds the maximum of %d (value will be capped)", a, maxAnisotropy);
		a = maxAnisotropy;
	}

	glTexParameterf(getTextureTypeEnum(type), GL_TEXTURE_MAX_ANISOTROPY, a);

}



void Texture::setMinFilter(TextureFilter filter, bool mipmapped) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set texture min filter)", id);

	switch (filter) {

		case TextureFilter::None:
			glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_MIN_FILTER, mipmapped ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
			break;

		case TextureFilter::Trilinear:

			if (!mipmapped) {
#if !GLE_TEXTURE_MERGE_FILTERS
				gle_assert("Attempted to set minifying filter to trilinear in non-mipmapped mode. To auto-merge modes, enable GLE_TEXTURE_MERGE_FILTERS.");
				//If we're not in debug mode and the program does not exit here, enable standard bilinear filtering
#endif
				glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			} else {
				glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			}

			break;

		case TextureFilter::Bilinear:
			glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_MIN_FILTER, mipmapped ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR);
			break;

	}

}



void Texture::setMagFilter(TextureFilter filter) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set texture mag filter)", id);

	switch (filter) {

		case TextureFilter::None:
			glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;

		case TextureFilter::Trilinear:
#if !GLE_TEXTURE_MERGE_FILTERS
			gle_assert("Attempted to set magnifying filter to trilinear. To auto-merge modes, enable GLE_TEXTURE_MERGE_FILTERS.");
			//If we're not in debug mode and the program does not exit here, enable standard bilinear filtering
#endif
		case TextureFilter::Bilinear:
			glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

	}

}



CubemapFace Texture::getCubemapFace(u32 index) {
	gle_assert(index < 6, "Invalid cubemap face index %d", index);
	return static_cast<CubemapFace>(index);
}



void Texture::setWrapU(TextureWrap wrap) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set wrap mode)", id);

	glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_WRAP_S, getTextureWrapEnum(wrap));

}



void Texture::setWrapV(TextureWrap wrap) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set wrap mode)", id);

	glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_WRAP_T, getTextureWrapEnum(wrap));

}



void Texture::setWrapW(TextureWrap wrap) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set wrap mode)", id);

	glTexParameteri(getTextureTypeEnum(type), GL_TEXTURE_WRAP_R, getTextureWrapEnum(wrap));

}



void Texture::generateMipmaps() {
	
	gle_assert(isBound(), "Texture %d has not been bound (attempted to generate mipmaps)", id);
	
	glGenerateMipmap(getTextureTypeEnum(type));

}



TextureType Texture::getTextureType() const {
	return type;
}



u32 Texture::getTextureTypeEnum(TextureType type) {

	switch (type) {

		case TextureType::Texture1D:
			return GL_TEXTURE_1D;

		case TextureType::Texture2D:
			return GL_TEXTURE_2D;

		case TextureType::Texture3D:
			return GL_TEXTURE_3D;

		case TextureType::ArrayTexture1D:
			return GL_TEXTURE_1D_ARRAY;

		case TextureType::ArrayTexture2D:
			return GL_TEXTURE_2D_ARRAY;

		case TextureType::CubemapTexture:
			return GL_TEXTURE_CUBE_MAP;

		case TextureType::CubemapArrayTexture:
			return GL_TEXTURE_CUBE_MAP_ARRAY;

		case TextureType::MultisampleTexture2D:
			return GL_TEXTURE_2D_MULTISAMPLE;

		case TextureType::MultisampleArrayTexture2D:
			return GL_TEXTURE_2D_MULTISAMPLE_ARRAY;

		default:
			gle_force_assert("Invalid texture type 0x%X", type);
			return -1;

	}

}



u32 Texture::getTextureWrapEnum(TextureWrap wrap) {

	switch (wrap) {

		case TextureWrap::Clamp:
			return GL_CLAMP_TO_EDGE;

		case TextureWrap::Mirror:
			return GL_MIRRORED_REPEAT;

		case TextureWrap::Repeat:
			return GL_REPEAT;

		default:
			gle_force_assert("Invalid texture wrap 0x%X", wrap);
			return -1;

	}

}



u32 Texture::getTextureSourceFormatEnum(TextureSourceFormat format) {

	switch (format) {

		case TextureSourceFormat::Red:
			return GL_RED;

		case TextureSourceFormat::Green:
			return GL_GREEN;

		case TextureSourceFormat::Blue:
			return GL_BLUE;

		case TextureSourceFormat::RG:
			return GL_RG;

		case TextureSourceFormat::RGB:
			return GL_RGB;

		case TextureSourceFormat::BGR:
			return GL_BGR;

		case TextureSourceFormat::RGBA:
			return GL_RGBA;

		case TextureSourceFormat::BGRA:
			return GL_BGRA;

		case TextureSourceFormat::Ri:
			return GL_RED_INTEGER;

		case TextureSourceFormat::Gi:
			return GL_GREEN_INTEGER;

		case TextureSourceFormat::Bi:
			return GL_BLUE_INTEGER;

		case TextureSourceFormat::RGi:
			return GL_RG_INTEGER;

		case TextureSourceFormat::RGBi:
			return GL_RGB_INTEGER;

		case TextureSourceFormat::BGRi:
			return GL_BGR_INTEGER;

		case TextureSourceFormat::RGBAi:
			return GL_RGBA_INTEGER;

		case TextureSourceFormat::BGRAi:
			return GL_BGRA_INTEGER;

		case TextureSourceFormat::Depth:
			return GL_DEPTH_COMPONENT;

		case TextureSourceFormat::Stencil:
			return GL_STENCIL_INDEX;

		case TextureSourceFormat::DepthStencil:
			return GL_DEPTH_STENCIL;

		default:
			gle_force_assert("Invalid texture source format 0x%X", format);
			return -1;

	}

}



u32 Texture::getTextureSourceTypeEnum(TextureSourceType type) {

	switch (type) {

		case TextureSourceType::UByte:
			return GL_UNSIGNED_BYTE;

		case TextureSourceType::Byte:
			return GL_BYTE;

		case TextureSourceType::UShort:
			return GL_UNSIGNED_SHORT;

		case TextureSourceType::Short:
			return GL_SHORT;

		case TextureSourceType::UInt:
			return GL_UNSIGNED_INT;

		case TextureSourceType::Int:
			return GL_INT;

		case TextureSourceType::Float:
			return GL_FLOAT;

		case TextureSourceType::UByte332:
			return GL_UNSIGNED_BYTE_3_3_2;

		case TextureSourceType::UByte233R:
			return GL_UNSIGNED_BYTE_2_3_3_REV;

		case TextureSourceType::UShort565:
			return GL_UNSIGNED_SHORT_5_6_5;

		case TextureSourceType::UShort565R:
			return GL_UNSIGNED_SHORT_5_6_5_REV;

		case TextureSourceType::UShort4444:
			return GL_UNSIGNED_SHORT_4_4_4_4;

		case TextureSourceType::UShort4444R:
			return GL_UNSIGNED_SHORT_4_4_4_4_REV;

		case TextureSourceType::UShort5551:
			return GL_UNSIGNED_SHORT_5_5_5_1;

		case TextureSourceType::UShort1555R:
			return GL_UNSIGNED_SHORT_1_5_5_5_REV;

		case TextureSourceType::UInt8888:
			return GL_UNSIGNED_INT_8_8_8_8;

		case TextureSourceType::UInt8888R:
			return GL_UNSIGNED_INT_8_8_8_8_REV;

		case TextureSourceType::UInt10_2:
			return GL_UNSIGNED_INT_10_10_10_2;

		case TextureSourceType::UInt2_10R:
			return GL_UNSIGNED_INT_2_10_10_10_REV;

		default:
			gle_force_assert("Invalid texture source type 0x%X", type);
			return -1;

	}

}



u32 Texture::getCubemapFaceEnum(CubemapFace face) {
	return GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<u32>(face);
}


GLE_END