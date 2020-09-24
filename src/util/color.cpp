#include "color.h"

u8 RGBA16::nAlphaThreshold = 0x7F;

RGBA32::RGBA32(RGB24 rgb) {
	rgba = rgb.rgb;
	a = 0xFF;
}

RGBA32::RGBA32(RGBA16 rgb) {
	r = (rgb.r << 3) | (rgb.r >> 2);
	g = (rgb.g << 3) | (rgb.g >> 2);
	b = (rgb.b << 3) | (rgb.b >> 2);
	a = rgb.a ? 0xFF : 0x0;
}

RGBA16::RGBA16(RGB15 rgb) {
	rgba = rgb.rgb;
	a = 1;
}

RGBA16::RGBA16(RGBA32 rgb) {
	r = rgb.r >> 3;
	g = rgb.g >> 3;
	b = rgb.b >> 3;
	a = rgb.a <= nAlphaThreshold ? 0 : 1;
}