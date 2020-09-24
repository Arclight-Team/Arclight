#pragma once
#include "../types.h"

class RGB24;
class RGBA32;
class RGB15;
class RGBA16;

class RGBA32 {
public:

	union {
		u32 rgba;
		struct {
			u8 a, b, g, r;
		};
	};

	constexpr RGBA32() : rgba(0) {}

	constexpr RGBA32(u32 rgba) : rgba(rgba) {}

	constexpr RGBA32(u8 gray, u8 a) : a(a), b(gray), g(gray), r(gray) {}

	constexpr RGBA32(u8 r, u8 g, u8 b, u8 a = 0xFF) : a(a), b(b), g(g), r(r) {}

	RGBA32(RGB24 rgb);

	RGBA32(RGBA16 rgba);

};

class RGBA16 {
public:

	union {
		u16 rgba;
		struct {
			u16 r : 5;
			u16 g : 5;
			u16 b : 5;
			u16 a : 1;
		};
	};

	static u8 nAlphaThreshold;

	constexpr RGBA16() : rgba(0) {}

	constexpr RGBA16(u16 rgba) : rgba(rgba) {}

	constexpr RGBA16(u8 gray, u8 a) : a(a), b(gray), g(gray), r(gray) {}

	constexpr RGBA16(u8 r, u8 g, u8 b, u8 a) : a(a), b(b), g(g), r(r) {}

	RGBA16(RGB15 rgb);

	RGBA16(RGBA32 rgba);

};

class RGB24 {
public:

	union {
		u32 rgb;
		struct {
			u8 _, b, g, r;
		};
	};

	constexpr RGB24() : rgb(0) {}

	constexpr RGB24(u32 rgb) : rgb(rgb), _(0) {}

	constexpr RGB24(u8 r, u8 g, u8 b) : b(b), g(g), r(r), _(0) {}

	constexpr RGB24(RGBA32 rgba) : rgb(rgba.rgba), _(0) {}

};

class RGB15 {
public:

	union {
		u16 rgb;
		struct {
			u16 r : 5;
			u16 g : 5;
			u16 b : 5;
			u16 _ : 1;
		};
	};

	constexpr RGB15() : rgb(0) {}

	constexpr explicit RGB15(u16 rgb) : r(0), g(0), b(0), rgb(rgb), _(0) {}

	constexpr RGB15(u8 r, u8 g, u8 b) : b(b), g(g), r(r), _(0) {}

	constexpr RGB15(RGBA16 rgba) : rgb(rgba.rgba), _(0) {
	}

};