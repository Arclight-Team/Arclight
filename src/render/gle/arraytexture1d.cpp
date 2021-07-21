#include "arraytexture1d.h"

#include "glecore.h"
#include GLE_HEADER


GLE_BEGIN


void ArrayTexture1D::setData(u32 w, u32 layers, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (w > Limits::getMaxTextureSize()) {
		error("1D array texture dimension of size %d exceeds maximum texture size of %d", w, Limits::getMaxTextureSize());
		return;
	}

	if (layers > Limits::getMaxArrayTextureLayers()) {
		error("1D array texture layer count of %d exceeds maximum array layer count of %d", w, Limits::getMaxArrayTextureLayers());
		return;
	}

	width = w;
	height = layers;
	depth = 0;
	texFormat = format;

	glTexImage2D(getTextureTypeEnum(type), 0, Image::getImageFormatEnum(texFormat), w, layers, 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void ArrayTexture1D::setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexImage2D(getTextureTypeEnum(type), level, Image::getImageFormatEnum(texFormat), getMipmapSize(level, width), height, 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void ArrayTexture1D::update(u32 x, u32 w, u32 layerStart, u32 layerCount, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to update data)", id);

	if ((x + w) > getMipmapSize(level, width)) {
		error("Updating 1D array texture out of bounds: width = %d, requested: x = %d, w = %d", getMipmapSize(level, width), x, w);
		return;
	}

	if ((layerStart + layerCount) > height) {
		error("Updating 1D array texture out of bounds: array size = %d, requested: start = %d, count = %d", height, layerStart, layerCount);
		return;
	}

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexSubImage2D(getTextureTypeEnum(type), level, x, layerStart, w, layerCount, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}


GLE_END