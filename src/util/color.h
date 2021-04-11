#pragma once

#include "arcconfig.h"
#include "types.h"
#include "vector.h"



struct ColorRGB {

	constexpr ColorRGB() : ColorRGB(0) {}
	constexpr explicit ColorRGB(u8 v) : ColorRGB(v, v, v) {}
	constexpr ColorRGB(u8 r, u8 g, u8 b) : r(r), g(g), b(b) {}

	constexpr Vec3f toVec3f() const {
		return Vec3f(r / 255.0, g / 255.0, b / 255.0);
	}

	constexpr Vec3i toVec3i() const {
		return Vec3i(r, g, b);
	}

	u8 r;
	u8 g;
	u8 b;

};



struct ColorRGBA : public ColorRGB {

	constexpr ColorRGBA() : ColorRGBA(0) {}
	constexpr explicit ColorRGBA(u8 v) : ColorRGBA(v, v, v, v) {}
	constexpr ColorRGBA(u8 r, u8 g, u8 b, u8 a) : ColorRGB(r, g, b), a(a) {}

	constexpr Vec4f toVec4f() const {
		return Vec4f(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
	}

	constexpr Vec4i toVec4i() const {
		return Vec4i(r, g, b, a);
	}

	u8 a;

};



struct ColorHSV {

	constexpr ColorHSV() : ColorHSV(0) {}
	constexpr explicit ColorHSV(double v) : ColorHSV(v, v, v) {}
	constexpr ColorHSV(double h, double s, double v) : h(h), s(s), v(v) {}

	constexpr bool operator==(const ColorHSV& color) {
		return Math::isEqual(s, color.s) && Math::isEqual(v, color.v) && Math::isEqual(Math::mod(h, 360.0), Math::mod(color.h, 360.0));
	}

	constexpr Vec3d toVec3d() const {
		return Vec3d(h, s, v);
	}

	double h;
	double s;
	double v;

};



namespace Color {

	//Quantize to u8 by scaling
	constexpr u8 quantizeScale8(double value) {
		return value * 255;
	}

	//Quantize to u8 by rounding
	constexpr u8 quantizeRound8(double value) {
		return Math::round(value * 255.0);
	}



	//Converts an RGB color to an HSV color
	constexpr ColorHSV toHSV(const ColorRGB& rgb) {

		i32 r = rgb.r;
		i32 g = rgb.g;
		i32 b = rgb.b;
		u8 mx = Math::max(r, g, b);
		u8 mn = Math::min(r, g, b);

		double h = 0.0;
		double s = 0.0;
		double v = mx / 255.0;

		if (mx) {
			s = (mx - mn) / static_cast<double>(mx);
		}

		if (mx != mn) {

			double d = mx - mn;

			if (mx == r) {
				h = 60 * (g - b) / d;
				h = (h >= 0) ? h : h + 360;
			} else if (mx == g) {
				h = 60 * (2 + (b - r) / d);
			} else {
				h = 60 * (4 + (r - g) / d);
			}

		}

		return ColorHSV(h, s, v);

	}



	//Converts an HSV color to an RGB color. Hue may exceed 360°.
	constexpr ColorRGB toRGB(const ColorHSV& hsv) {

		double h = hsv.h;
		double s = hsv.s;
		double v = hsv.v;

		if (Math::isZero(s)) {
			return ColorRGB(ARC_COLOR_QUANTIZE(v));
		}

		double r = h / 60.0;
		u32 i = r;

		double f = r - i;
		double p = v * (1.0 - s);
		double q = v * (1.0 - s * f);
		double t = v * (1.0 - s * (1.0 - f));

		switch (i % 6) {

			case 0:
				return ColorRGB(ARC_COLOR_QUANTIZE(v), ARC_COLOR_QUANTIZE(t), ARC_COLOR_QUANTIZE(p));
			case 1:
				return ColorRGB(ARC_COLOR_QUANTIZE(q), ARC_COLOR_QUANTIZE(v), ARC_COLOR_QUANTIZE(p));
			case 2:
				return ColorRGB(ARC_COLOR_QUANTIZE(p), ARC_COLOR_QUANTIZE(v), ARC_COLOR_QUANTIZE(t));
			case 3:
				return ColorRGB(ARC_COLOR_QUANTIZE(p), ARC_COLOR_QUANTIZE(q), ARC_COLOR_QUANTIZE(v));
			case 4:
				return ColorRGB(ARC_COLOR_QUANTIZE(t), ARC_COLOR_QUANTIZE(p), ARC_COLOR_QUANTIZE(v));
			case 5:
				return ColorRGB(ARC_COLOR_QUANTIZE(v), ARC_COLOR_QUANTIZE(p), ARC_COLOR_QUANTIZE(q));

		}

		return ColorRGB();

	}



	//Linear interpolation across RGB values
	constexpr Vec3f rgbLerp(ColorRGB start, ColorRGB end, double factor) {
		return ColorRGB(Math::lerp(start.r, end.r, factor),
						Math::lerp(start.g, end.g, factor),
						Math::lerp(start.b, end.b, factor)).toVec3f();
	}



	//Adjusts the Hue for fast linear interpolation
	constexpr void initFastHSVLerp(ColorHSV& start, ColorHSV& end) {

		double sh = start.h;
		double eh = end.h;

		if (sh > eh) {

			if ((sh - eh) >= 180.0) {
				end.h += 360.0;
			}

		} else {

			if ((eh - sh) >= 180.0) {
				start.h += 360.0;
			}

		}

	}


	//Fast spherical linear interpolation across HSV values. Hue must be normalized to [0; 360).
	constexpr Vec3f fastHSVSlerp(const ColorHSV& start, const ColorHSV& end, double factor) {
		return Color::toRGB(ColorHSV(Math::lerp(start.h, end.h, factor),
									 Math::lerp(start.s, end.s, factor),
									 Math::lerp(start.v, end.v, factor))).toVec3f();
	}




	constexpr u32 packRGBA(u8 r, u8 g, u8 b, u8 a = 0xFF) noexcept {
		return (a << 24) | (b << 16) | (g << 8) | r;
	}



	constexpr void unpackRGBA(u32 rgba, u8& r, u8& g, u8& b, u8& a) noexcept {

		b = rgba & 0xFF;
		g = (rgba >> 8) & 0xFF;
		r = (rgba >> 16) & 0xFF;
		a = (rgba >> 24) & 0xFF;

	}



	constexpr u16 packRGB15(u8 r, u8 g, u8 b) noexcept {
		return ((b & 0x1F) << 10) | ((g & 0x1F) << 5) | (r & 0x1F);
	}



	constexpr void unpackRGB15(u16 rgb, u8& r, u8& g, u8& b) noexcept {

		r = rgb & 0x1F;
		g = (rgb >> 5) & 0x1F;
		b = (rgb >> 10) & 0x1F;

	}



	constexpr auto convertRGB15ToRGBA(u16 rgb15) noexcept {

		u8 r, g, b;

		unpackRGB15(rgb15, r, g, b);

		r = (r << 3) | (r >> 2);
		g = (g << 3) | (g >> 2);
		b = (b << 3) | (b >> 2);

		return packRGBA(r, g, b);

	}



	constexpr auto convertRGBAToRGB15(u16 rgb15) noexcept {

		u8 r, g, b, a;

		unpackRGBA(rgb15, r, g, b, a);

		r = r >> 3;
		g = g >> 3;
		b = b >> 3;

		return packRGB15(r, g, b);

	}



	constexpr auto convertRGB15ToColor(u16 rgb15) noexcept {

		u8 r, g, b;

		unpackRGB15(rgb15, r, g, b);

		r = (r << 3) | (r >> 2);
		g = (g << 3) | (g >> 2);
		b = (b << 3) | (b >> 2);

		return ColorRGBA(r, g, b, 0xFF);

	}


}
