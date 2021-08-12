#include "texture3d.h"

#include "glecore.h"
#include GLE_HEADER


GLE_BEGIN


void Texture3D::setData(u32 w, u32 h, u32 d, ImageFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (w > Limits::getMaxTextureSize() || h > Limits::getMaxTextureSize() || d > Limits::getMax3DTextureSize()) {
		error("3D texture dimension of size %d exceeds maximum texture size of %d", (w > h ? (d > w ? d : w) : (d > h ? d : h)), Limits::getMaxTextureSize());
		return;
	}

	width = w;
	height = h;
	depth = d;
	texFormat = format;

	glTexImage3D(getTextureTypeEnum(type), 0, Image::getImageFormatEnum(texFormat), w, h, d, 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void Texture3D::setMipmapData(u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexImage3D(getTextureTypeEnum(type), level, Image::getImageFormatEnum(texFormat), getMipmapSize(level, width), getMipmapSize(level, height), getMipmapSize(level, depth), 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void Texture3D::update(u32 x, u32 y, u32 z, u32 w, u32 h, u32 d, TextureSourceFormat srcFormat, TextureSourceType srcType, const void* data, u32 level) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to update data)", id);

	if ((x + w) > getMipmapSize(level, width)) {
		error("Updating 3D texture out of bounds: width = %d, requested: x = %d, w = %d", getMipmapSize(level, width), x, w);
		return;
	}

	if ((y + h) > getMipmapSize(level, height)) {
		error("Updating 3D texture out of bounds: height = %d, requested: y = %d, h = %d", getMipmapSize(level, height), y, h);
		return;
	}

	if ((z + d) > getMipmapSize(level, depth)) {
		error("Updating 3D texture out of bounds: depth = %d, requested: z = %d, d = %d", getMipmapSize(level, depth), z, d);
		return;
	}

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexSubImage3D(getTextureTypeEnum(type), level, x, y, z, w, h, d, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void Texture3D::setCompressedData(u32 w, u32 h, u32 d, CompressedImageFormat format, const void* data, u32 size) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (w > Limits::getMaxTextureSize() || h > Limits::getMaxTextureSize() || d > Limits::getMax3DTextureSize()) {
		error("3D texture dimension of size %d exceeds maximum texture size of %d", (w > h ? (d > w ? d : w) : (d > h ? d : h)), Limits::getMaxTextureSize());
		return;
	}

	width = w;
	height = h;
	depth = d;
	texFormat = static_cast<ImageFormat>(format);

	glCompressedTexImage3D(getTextureTypeEnum(type), 0, Image::getCompressedImageFormatEnum(format), w, h, d, 0, size, data);

}



void Texture3D::setCompressedMipmapData(u32 level, const void* data, u32 size) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	auto format = getCompressedImageFormat();

	glCompressedTexImage3D(getTextureTypeEnum(type), level, Image::getCompressedImageFormatEnum(format), getMipmapSize(level, width), getMipmapSize(level, height), getMipmapSize(level, depth), 0, size, data);

}


GLE_END