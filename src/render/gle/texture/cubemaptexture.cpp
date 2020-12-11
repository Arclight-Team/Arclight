#include "cubemaptexture.h"

#include "../glecore.h"
#include GLE_HEADER


GLE_BEGIN


void CubemapTexture::setData(CubemapFace face, u32 s, TextureFormat format, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set data)", id);

	if (s > Limits::getMaxTextureSize()) {
		GLE::error("2D cubemap texture dimension of size %d exceeds maximum texture size of %d", s, Limits::getMaxTextureSize());
		return;
	}

	if (texFormat == TextureFormat::None) {

		width = s;
		height = s;
		depth = 0;
		texFormat = format;

	} else if (texFormat != format || width != s || height != s) {
		GLE::error("Cubemap initialization inconsistent");
		return;
	}

	glTexImage2D(getCubemapFaceEnum(face), 0, getTextureFormatEnum(texFormat), s, s, 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void CubemapTexture::setMipmapData(CubemapFace face, u32 level, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data) {

	gle_assert(isBound(), "Texture %d has not been bound (attempted to set mipmap data)", id);

	if (level > getMipmapCount()) {
		GLE::error("Specified mipmap level %d which exceeds the total mipmap count of %d", level, getMipmapCount());
		return;
	}

	glTexImage2D(getCubemapFaceEnum(face), level, getTextureFormatEnum(texFormat), getMipmapSize(level, width), getMipmapSize(level, height), 0, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}



void CubemapTexture::update(CubemapFace face, u32 x, u32 y, u32 w, u32 h, TextureSourceFormat srcFormat, TextureSourceType srcType, void* data, u32 level) {

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

	glTexSubImage2D(getCubemapFaceEnum(face), level, x, y, w, h, getTextureSourceFormatEnum(srcFormat), getTextureSourceTypeEnum(srcType), data);

}


GLE_END