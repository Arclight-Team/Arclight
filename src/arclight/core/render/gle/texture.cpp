/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 texture.cpp
 */

#include "texture.hpp"

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
		glBindTexture(glTextureTypeEnum[u32(type)], id);
		setBoundTextureID(type, id);
	}

}



void Texture::unbind() {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to unbind)", id);

	setBoundTextureID(type, invalidBoundID);

	glBindTexture(glTextureTypeEnum[u32(type)], 0);

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

	gle_assert(isCreated(), "Texture hasn't been created yet");

	activateUnit(unit);

	//Force-bind
	glBindTexture(glTextureTypeEnum[u32(type)], id);
	setBoundTextureID(type, id);

}



void Texture::activateUnit(u32 unit) {

	gle_assert(unit < Limits::getMaxCombinedTextureUnits(), "Texture unit %d exceeds the maximum of %d", unit, Limits::getMaxCombinedTextureUnits());

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



CompressedImageFormat Texture::getCompressedImageFormat() const {
	return static_cast<CompressedImageFormat>(texFormat);
}



u32 Texture::getMipmapSize(u32 level, u32 d) {
	return (d >> level) ? d >> level : 1U;
}



void Texture::setMipmapBaseLevel(u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set base mipmap level)", id);

	glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_BASE_LEVEL, level);

}



void Texture::setMipmapMaxLevel(u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set max mipmap level)", id);

	glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_MAX_LEVEL, level);

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

	glTexParameterf(glTextureTypeEnum[u32(type)], GL_TEXTURE_MAX_ANISOTROPY, a);

}



void Texture::setMinFilter(TextureFilter filter, bool mipmapped) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set texture min filter)", id);

	switch (filter) {

		case TextureFilter::None:
			glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_MIN_FILTER, mipmapped ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
			break;

		case TextureFilter::Trilinear:

			if (!mipmapped) {
#if !GLE_TEXTURE_MERGE_FILTERS
				gle_assert("Attempted to set minifying filter to trilinear in non-mipmapped mode. To auto-merge modes, enable GLE_TEXTURE_MERGE_FILTERS.");
				//If we're not in debug mode and the program does not exit here, enable standard bilinear filtering
#endif
				glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			} else {
				glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			}

			break;

		case TextureFilter::Bilinear:
			glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_MIN_FILTER, mipmapped ? GL_NEAREST_MIPMAP_LINEAR : GL_LINEAR);
			break;

	}

}



void Texture::setMagFilter(TextureFilter filter) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set texture mag filter)", id);

	switch (filter) {

		case TextureFilter::None:
			glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;

		case TextureFilter::Trilinear:
#if !GLE_TEXTURE_MERGE_FILTERS
			gle_assert("Attempted to set magnifying filter to trilinear. To auto-merge modes, enable GLE_TEXTURE_MERGE_FILTERS.");
			//If we're not in debug mode and the program does not exit here, enable standard bilinear filtering
#endif
		case TextureFilter::Bilinear:
			glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

	}

}



void Texture::enableComparisonMode(TextureOperator op) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to enable texture comparison)", id);

	glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_COMPARE_FUNC, static_cast<u32>(op));

}



void Texture::disableComparisonMode() {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to disable texture comparison)", id);

	glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_COMPARE_MODE, GL_NONE);

}



CubemapFace Texture::getCubemapFace(u32 index) {
	gle_assert(index < 6, "Invalid cubemap face index %d", index);
	return static_cast<CubemapFace>(index);
}



void Texture::setWrapU(TextureWrap wrap) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set wrap mode)", id);

	glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_WRAP_S, static_cast<u32>(wrap));

}



void Texture::setWrapV(TextureWrap wrap) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set wrap mode)", id);

	glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_WRAP_T, static_cast<u32>(wrap));

}



void Texture::setWrapW(TextureWrap wrap) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set wrap mode)", id);

	glTexParameteri(glTextureTypeEnum[u32(type)], GL_TEXTURE_WRAP_R, static_cast<u32>(wrap));

}



void Texture::setBorderColor(float r, float g, float b, float a) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set border color)", id);

	float color[4] = {r, g, b, a};
	glTexParameterfv(glTextureTypeEnum[u32(type)], GL_TEXTURE_BORDER_COLOR, color);

}



void Texture::generateMipmaps() {
	
	gle_assert(isBound(), "Texture %d has not been bound (attempted to generate mipmaps)", id);
	
	glGenerateMipmap(glTextureTypeEnum[u32(type)]);

}



void Texture::bindImageUnit(u32 unit, bool layered, u32 layer, Access access, u32 level) {

	gle_assert(Image::isImageUnitCompatible(texFormat), "Texture %d has an image unit incompatible format (attempted to bind image unit)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glBindImageTexture(unit, id, level, layered, layer, static_cast<u32>(access), static_cast<u32>(texFormat));

}



TextureType Texture::getTextureType() const {
	return type;
}


GLE_END