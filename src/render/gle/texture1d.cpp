#include "texture1d.h"

#include "glecore.h"
#include GLE_HEADER


GLE_BEGIN



void Texture1D::setData(u32 w, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (w > Limits::getMaxTextureSize()) {
		error("1D texture dimension of size %d exceeds maximum texture size of %d", w, Limits::getMaxTextureSize());
		return;
	}

	width = w;
	height = 0;
	depth = 0;
	texFormat = format;

	glTexImage1D(getTextureTypeEnum(type), 0, Image::getImageFormatEnum(texFormat), w, 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void Texture1D::setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexImage1D(getTextureTypeEnum(type), level, Image::getImageFormatEnum(texFormat), getMipmapSize(level, width), 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void Texture1D::update(u32 x, u32 w, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to update data)", id);

	if ((x + w) > getMipmapSize(level, width)) {
		error("Updating 1D texture out of bounds: width = %d, requested: x = %d, w = %d", getMipmapSize(level, width), x, w);
		return;
	}

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexSubImage1D(getTextureTypeEnum(type), level, x, w, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



GLE_END