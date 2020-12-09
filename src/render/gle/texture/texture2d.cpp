#include "texture2d.h"

#include "../glecore.h"
#include GLE_HEADER


GLE_BEGIN


void Texture2D::setData(u32 w, u32 h, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (w > Core::getMaxTextureSize() || h > Core::getMaxTextureSize()) {
		GLE::error("2D texture dimension of size %d exceeds maximum texture size of %d", (w > h ? w : h), Core::getMaxTextureSize());
		return;
	}

	width = w;
	height = h;
	depth = 0;
	texFormat = format;

	glTexImage2D(getTextureTypeEnum(type), 0, getTextureFormatEnum(texFormat), w, h, 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void Texture2D::setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		GLE::error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexImage2D(getTextureTypeEnum(type), level, getTextureFormatEnum(texFormat), getMipmapSize(level, width), getMipmapSize(level, height), 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void Texture2D::update(u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to update data)", id);

	if ((x + w) > getMipmapSize(level, width)) {
		GLE::error("Updating 2D texture out of bounds: width = %d, requested: x = %d, w = %d", getMipmapSize(level, width), x, w);
		return;
	}

	if ((y + h) > getMipmapSize(level, height)) {
		GLE::error("Updating 2D texture out of bounds: height = %d, requested: y = %d, h = %d", getMipmapSize(level, height), y, h);
		return;
	}

	if (level > getMipmapCount()) {
		GLE::error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexSubImage2D(getTextureTypeEnum(type), level, x, y, w, h, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}


GLE_END