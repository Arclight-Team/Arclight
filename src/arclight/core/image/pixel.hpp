/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 pixel.hpp
 */

#pragma once

#include "math/math.hpp"
#include "util/assert.hpp"
#include "util/bits.hpp"
#include "common/typetraits.hpp"
#include "arcconfig.hpp"
#include "types.hpp"

#include <algorithm>
#include <span>



//Pixel Types
enum class Pixel {
	Grayscale8,
	BGR5,
	RGB5,
	BGR8,
	RGB8,
	RGBA8,
	ABGR8,
	BGRA8,
	ARGB8
};


//Pixel Formats
template<Pixel P>
struct PixelFormat {};


template<>
struct PixelFormat<Pixel::Grayscale8> {

	constexpr static u32 BytesPerPixel = 1;
	constexpr static u32 Channels = 1;
	constexpr static u32 RedMask    = 0x000000FF;
	constexpr static u32 GreenMask  = 0x00000000;
	constexpr static u32 BlueMask   = 0x00000000;
	constexpr static u32 AlphaMask  = 0x00000000;
	constexpr static u32 RedShift   = 0;
	constexpr static u32 GreenShift = 0;
	constexpr static u32 BlueShift  = 0;
	constexpr static u32 AlphaShift = 0;

};


template<>
struct PixelFormat<Pixel::RGBA8> {

	constexpr static u32 BytesPerPixel = 4;
	constexpr static u32 Channels = 4;
	constexpr static u32 RedMask    = 0x000000FF;
	constexpr static u32 GreenMask  = 0x0000FF00;
	constexpr static u32 BlueMask   = 0x00FF0000;
	constexpr static u32 AlphaMask  = 0xFF000000;
	constexpr static u32 RedShift   = 0;
	constexpr static u32 GreenShift = 8;
	constexpr static u32 BlueShift  = 16;
	constexpr static u32 AlphaShift = 24;

};


template<>
struct PixelFormat<Pixel::ABGR8> {

	constexpr static u32 BytesPerPixel = 4;
	constexpr static u32 Channels = 4;
	constexpr static u32 RedMask    = 0xFF000000;
	constexpr static u32 GreenMask  = 0x00FF0000;
	constexpr static u32 BlueMask   = 0x0000FF00;
	constexpr static u32 AlphaMask  = 0x000000FF;
	constexpr static u32 RedShift   = 24;
	constexpr static u32 GreenShift = 16;
	constexpr static u32 BlueShift  = 8;
	constexpr static u32 AlphaShift = 0;

};


template<>
struct PixelFormat<Pixel::BGRA8> {

	constexpr static u32 BytesPerPixel = 4;
	constexpr static u32 Channels = 4;
	constexpr static u32 RedMask    = 0x00FF0000;
	constexpr static u32 GreenMask  = 0x0000FF00;
	constexpr static u32 BlueMask   = 0x000000FF;
	constexpr static u32 AlphaMask  = 0xFF000000;
	constexpr static u32 RedShift   = 16;
	constexpr static u32 GreenShift = 8;
	constexpr static u32 BlueShift  = 0;
	constexpr static u32 AlphaShift = 24;

};



template<>
struct PixelFormat<Pixel::ARGB8> {

	constexpr static u32 BytesPerPixel = 4;
	constexpr static u32 Channels = 4;
	constexpr static u32 RedMask    = 0x0000FF00;
	constexpr static u32 GreenMask  = 0x00FF0000;
	constexpr static u32 BlueMask   = 0xFF000000;
	constexpr static u32 AlphaMask  = 0x000000FF;
	constexpr static u32 RedShift   = 8;
	constexpr static u32 GreenShift = 16;
	constexpr static u32 BlueShift  = 24;
	constexpr static u32 AlphaShift = 0;

};


template<>
struct PixelFormat<Pixel::RGB8> {

	constexpr static u32 BytesPerPixel = 3;
	constexpr static u32 Channels = 3;
	constexpr static u32 RedMask    = 0x000000FF;
	constexpr static u32 GreenMask  = 0x0000FF00;
	constexpr static u32 BlueMask   = 0x00FF0000;
	constexpr static u32 AlphaMask  = 0x00000000;
	constexpr static u32 RedShift   = 0;
	constexpr static u32 GreenShift = 8;
	constexpr static u32 BlueShift  = 16;
	constexpr static u32 AlphaShift = 0;

};


template<>
struct PixelFormat<Pixel::BGR8> {

	constexpr static u32 BytesPerPixel = 3;
	constexpr static u32 Channels = 3;
	constexpr static u32 RedMask    = 0x00FF0000;
	constexpr static u32 GreenMask  = 0x0000FF00;
	constexpr static u32 BlueMask   = 0x000000FF;
	constexpr static u32 AlphaMask  = 0x00000000;
	constexpr static u32 RedShift   = 16;
	constexpr static u32 GreenShift = 8;
	constexpr static u32 BlueShift  = 0;
	constexpr static u32 AlphaShift = 0;

};


template<>
struct PixelFormat<Pixel::RGB5> {

	constexpr static u32 BytesPerPixel = 2;
	constexpr static u32 Channels = 3;
	constexpr static u32 RedMask    = 0x0000001F;
	constexpr static u32 GreenMask  = 0x000003E0;
	constexpr static u32 BlueMask   = 0x00007C00;
	constexpr static u32 AlphaMask  = 0x00000000;
	constexpr static u32 RedShift   = 0;
	constexpr static u32 GreenShift = 5;
	constexpr static u32 BlueShift  = 10;
	constexpr static u32 AlphaShift = 0;

};


template<>
struct PixelFormat<Pixel::BGR5> {

	constexpr static u32 BytesPerPixel = 2;
	constexpr static u32 Channels = 3;
	constexpr static u32 RedMask    = 0x00007C00;
	constexpr static u32 GreenMask  = 0x000003E0;
	constexpr static u32 BlueMask   = 0x0000001F;
	constexpr static u32 AlphaMask  = 0x00000000;
	constexpr static u32 RedShift   = 10;
	constexpr static u32 GreenShift = 5;
	constexpr static u32 BlueShift  = 0;
	constexpr static u32 AlphaShift = 0;

};



//Pixel Storages
template<Pixel P, class ColorT>
struct PixelStorage {

public:

	constexpr static Pixel PixelType = P;
	using ColorType = ColorT;

	using Format = PixelFormat<P>;
	constexpr static u32 Size = Format::BytesPerPixel;
	using PackedT = TT::UnsignedFromMinSize<Size>;
	using PixelT = PixelStorage<P, ColorT>;

	constexpr PixelStorage() {
		std::fill(p, p + Size, 0);
	}

	constexpr explicit PixelStorage(const std::span<const u8>& data) {
		std::copy(data.begin(), data.begin() + Size, p);
	}

	constexpr explicit PixelStorage(PackedT color) {
		unpack(color);
	}

	constexpr explicit PixelStorage(const PixelT& t) {
		*this = t;
	}

	constexpr void setGrayscale(ColorT v) {
		setRed(v);
	}

	constexpr void setMonochrome(ColorT v) {
		setRGB(v, v, v);
	}

	constexpr void setRGB(ColorT r, ColorT g, ColorT b) {
		setRGBA(r, g, b, getAlpha());
	}

	constexpr void setRGBA(ColorT r, ColorT g, ColorT b, ColorT a) {
		unpack(r << Format::RedShift | g << Format::GreenShift | b << Format::BlueShift | a << Format::AlphaShift);
	}

	constexpr void setRed(ColorT r) {

		auto t = pack();
		t &= ~Format::RedMask;
		t |= r << Format::RedShift;
		unpack(t);

	}

	constexpr void setGreen(ColorT g) {

		auto t = pack();
		t &= ~Format::GreenMask;
		t |= g << Format::GreenShift;
		unpack(t);

	}

	constexpr void setBlue(ColorT b) {

		auto t = pack();
		t &= ~Format::BlueMask;
		t |= b << Format::BlueShift;
		unpack(t);

	}

	constexpr void setAlpha(ColorT a) {

		auto t = pack();
		t &= ~Format::AlphaMask;
		t |= a << Format::AlphaShift;
		unpack(t);

	}

	constexpr ColorT getRed() const {
		return (pack() & Format::RedMask) >> Format::RedShift;
	}

	constexpr ColorT getGreen() const {
		return (pack() & Format::GreenMask) >> Format::GreenShift;
	}

	constexpr ColorT getBlue() const {
		return (pack() & Format::BlueMask) >> Format::BlueShift;
	}

	constexpr ColorT getAlpha() const {
		return (pack() & Format::AlphaMask) >> Format::AlphaShift;
	}

	constexpr static ColorT getMaxRed() {
		return Format::RedMask >> Format::RedShift;
	}

	constexpr static ColorT getMaxGreen() {
		return Format::GreenMask >> Format::GreenShift;
	}

	constexpr static ColorT getMaxBlue() {
		return Format::BlueMask >> Format::BlueShift;
	}

	constexpr static ColorT getMaxAlpha() {
		return Format::AlphaMask >> Format::AlphaShift;
	}

	constexpr void unpack(PackedT t) {

		for(u32 i = 0; i < Size; i++) {
			p[i] = t & 0xFF;
			t >>= 8;
		}

	}
	
	constexpr PackedT pack() const {

		PackedT t = 0;

		for(u32 i = 0; i < Size; i++) {
			t |= p[i] << (i * 8);
		}

		return t;

	}

	constexpr void add(const PixelT& t) {

		auto r = getRed() + t.getRed();
		auto g = getGreen() + t.getGreen();
		auto b = getBlue() + t.getBlue();
		r = Math::min(r, getMaxRed());
		g = Math::min(g, getMaxGreen());
		b = Math::min(b, getMaxBlue());
		setRGB(r, g, b);

	}

	constexpr void subtract(const PixelT& t) {

		using SColor = TT::MakeSigned<ColorT>;

		SColor r = getRed() - t.getRed();
		SColor g = getGreen() - t.getGreen();
		SColor b = getBlue() - t.getBlue();
		r = Math::max(r, 0);
		g = Math::max(g, 0);
		b = Math::max(b, 0);
		setRGB(r, g, b);

	}

	constexpr void multiply(double f) {

		arc_assert(f > 0.0, "Cannot multiply pixel with a factor less than 0");

		auto r = f * getRed();
		auto g = f * getGreen();
		auto b = f * getBlue();
		r = Math::min(r, getMaxRed());
		g = Math::min(g, getMaxGreen());
		b = Math::min(b, getMaxBlue());
		
#ifdef ARC_PIXEL_EXACT
		setRGB(static_cast<u32>(Math::round(r)), static_cast<u32>(Math::round(g)), static_cast<u32>(Math::round(b)));
#else
		setRGB(static_cast<u32>(r), static_cast<u32>(g), static_cast<u32>(b));
#endif

	}

	constexpr void divide(double f) {

		arc_assert(f > 0.0 && !Math::isZero(f), "Cannot multiply pixel with a factor less than 0");
		multiply(1 / f);

	}

	constexpr PixelT& operator+=(const PixelT& t) {
		add(t);
		return *this;
	}

	constexpr PixelT& operator-=(const PixelT& t) {
		subtract(t);
		return *this;
	}

	constexpr PixelT& operator*=(double f) {
		multiply(f);
		return *this;
	}

	constexpr PixelT& operator/=(double f) {
		divide(f);
		return *this;
	}

	constexpr PixelT& operator=(const PixelT& t) {
		unpack(t.pack());
		return *this;
	}

	constexpr bool operator==(const PixelT& t) {
		return pack() == t.pack();
	}

	constexpr u8 operator[](SizeT i) const {
		return p[i];
	}

	u8 p[Size];

};


namespace CC {

	template<class T>
	concept PixelStorageType = CC::BaseOf<T, PixelStorage<T::PixelType, typename T::ColorType>>;

}


constexpr auto operator+(CC::PixelStorageType auto a, const CC::PixelStorageType auto& b) {
	a += b;
	return a;
}

constexpr auto operator-(CC::PixelStorageType auto a, const CC::PixelStorageType auto& b) {
	a -= b;
	return a;
}

constexpr auto operator*(CC::PixelStorageType auto a, double f) {
	a *= f;
	return a;
}

constexpr auto operator*(double f, CC::PixelStorageType auto a) {
	a *= f;
	return a;
}

constexpr auto operator/(CC::PixelStorageType auto a, double f) {
	a /= f;
	return a;
}



struct PixelGrayscale8 : public PixelStorage<Pixel::Grayscale8, u8> {

	constexpr PixelGrayscale8() : PixelStorage(0) {}

	constexpr PixelGrayscale8(u8 v) {
		setGrayscale(v);
	}

	using PixelStorage::PixelStorage;

};


struct PixelRGB5 : public PixelStorage<Pixel::RGB5, u8> {

	constexpr PixelRGB5() : PixelStorage(0) {}

	constexpr PixelRGB5(u8 r, u8 g, u8 b) {
		setRGB(r, g, b);
	}

	using PixelStorage::PixelStorage;

};


struct PixelBGR5 : public PixelStorage<Pixel::BGR5, u8> {

	constexpr PixelBGR5() : PixelStorage(0) {}

	constexpr PixelBGR5(u8 b, u8 g, u8 r) {
		setRGB(r, g, b);
	}

	using PixelStorage::PixelStorage;

};


struct PixelRGB8 : public PixelStorage<Pixel::RGB8, u8> {

	constexpr PixelRGB8() : PixelStorage(0) {}

	constexpr PixelRGB8(u8 r, u8 g, u8 b) {
		setRGB(r, g, b);
	}

	using PixelStorage::PixelStorage;

};


struct PixelBGR8 : public PixelStorage<Pixel::BGR8, u8> {

	constexpr PixelBGR8() : PixelStorage(0) {}

	constexpr PixelBGR8(u8 b, u8 g, u8 r) {
		setRGB(r, g, b);
	}

	using PixelStorage::PixelStorage;

};


struct PixelRGBA8 : public PixelStorage<Pixel::RGBA8, u8> {

	constexpr PixelRGBA8() : PixelStorage(0xFF000000) {}

	constexpr PixelRGBA8(u8 r, u8 g, u8 b, u8 a) {
		setRGBA(r, g, b, a);
	}

	using PixelStorage::PixelStorage;

};


struct PixelABGR8 : public PixelStorage<Pixel::ABGR8, u8> {

	constexpr PixelABGR8() : PixelStorage(0xFF) {}

	constexpr PixelABGR8(u8 a, u8 b, u8 g, u8 r) {
		setRGBA(r, g, b, a);
	}

	using PixelStorage::PixelStorage;

};


struct PixelBGRA8 : public PixelStorage<Pixel::BGRA8, u8> {

	constexpr PixelBGRA8() : PixelStorage(0xFF000000) {}

	constexpr PixelBGRA8(u8 b, u8 g, u8 r, u8 a) {
		setRGBA(r, g, b, a);
	}

	using PixelStorage::PixelStorage;

};


struct PixelARGB8 : public PixelStorage<Pixel::ARGB8, u8> {

	constexpr PixelARGB8() : PixelStorage(0xFF) {}

	constexpr PixelARGB8(u8 a, u8 r, u8 g, u8 b) {
		setRGBA(r, g, b, a);
	}

	using PixelStorage::PixelStorage;

};



template<Pixel P> struct _PixelType {};
template<> struct _PixelType<Pixel::Grayscale8> { using Type = PixelGrayscale8; };
template<> struct _PixelType<Pixel::RGB5> { using Type = PixelRGB5; };
template<> struct _PixelType<Pixel::BGR5> { using Type = PixelBGR5; };
template<> struct _PixelType<Pixel::RGB8> { using Type = PixelRGB8; };
template<> struct _PixelType<Pixel::BGR8> { using Type = PixelBGR8; };
template<> struct _PixelType<Pixel::RGBA8> { using Type = PixelRGBA8; };
template<> struct _PixelType<Pixel::ABGR8> { using Type = PixelABGR8; };
template<> struct _PixelType<Pixel::BGRA8> { using Type = PixelBGRA8; };
template<> struct _PixelType<Pixel::ARGB8> { using Type = PixelARGB8; };

template<Pixel P> using PixelType = typename _PixelType<P>::Type;



template<Pixel P>
class Colors
{

	using PixelT = PixelType<P>;
	using ColorT = typename PixelT::ColorType;
	using Format = typename PixelT::Format;
	
	constexpr static PixelT construct(float r, float g, float b, float a) {

		PixelT pixel;
		
		pixel.setRed	(r * PixelT::getMaxRed  ());
		pixel.setGreen	(g * PixelT::getMaxGreen());
		pixel.setBlue	(b * PixelT::getMaxBlue ());
		pixel.setAlpha	(a * PixelT::getMaxAlpha());

		return pixel;

	}

public:

	constexpr static PixelT Transparent	= construct(0.0, 0.0, 0.0, 0.0);
	constexpr static PixelT Black		= construct(0.0, 0.0, 0.0, 1.0);
	constexpr static PixelT White		= construct(1.0, 1.0, 1.0, 1.0);
	constexpr static PixelT Red			= construct(1.0, 0.0, 0.0, 1.0);
	constexpr static PixelT Green		= construct(0.0, 1.0, 0.0, 1.0);
	constexpr static PixelT Blue		= construct(0.0, 0.0, 1.0, 1.0);
	constexpr static PixelT Cyan		= construct(0.0, 1.0, 1.0, 1.0);
	constexpr static PixelT Yellow		= construct(1.0, 1.0, 0.0, 1.0);
	constexpr static PixelT Magenta		= construct(1.0, 0.0, 1.0, 1.0);

};


struct PixelConverter  {

private:

	template<class T, u32 InBits, u32 OutBits>
	constexpr static T convertChannel(T value) {

#ifdef ARC_PIXEL_EXACT
		if constexpr (InBits == 0) {
			return 0;
		}

		return static_cast<T>(Math::round(value * ((1 << OutBits) - 1) / static_cast<float>((1 << InBits) - 1)));
#else
		if constexpr (InBits > OutBits) {
			return value >> (InBits - OutBits);
		} else {
			return value << (OutBits - InBits);
		}
#endif

	}

	template<class T>
	constexpr static T convertChannel(T value, u32 inBits, u32 outBits) {

#ifdef ARC_PIXEL_EXACT
		return inBits ? static_cast<T>(Math::round(value * ((1 << outBits) - 1) / static_cast<float>((1 << inBits) - 1))) : 0;
#else
		return static_cast<TT::UnsignedFromMinSize<sizeof(T) + 1>>(value << outBits) >> inBits;
#endif

	}

public:

	template<Pixel DestPixel, class T>
	constexpr static auto convert(T&& pixel) {

		using U = TT::RemoveCVRef<T>;

		using SrcFormat = typename U::Format;
		using DestFormat = PixelFormat<DestPixel>;
		using DestPixelType = PixelType<DestPixel>;
		using SrcType = typename U::PackedT;
		using DestType = typename DestPixelType::PackedT;
		using ConvType = TT::Conditional<(sizeof(SrcType) > sizeof(DestType)), SrcType, DestType>;

		SrcType t = pixel.pack();

		constexpr u32 RBitsIn = Bits::popcount(SrcFormat::RedMask);
		constexpr u32 GBitsIn = Bits::popcount(SrcFormat::GreenMask);
		constexpr u32 BBitsIn = Bits::popcount(SrcFormat::BlueMask);
		constexpr u32 ABitsIn = Bits::popcount(SrcFormat::AlphaMask);
		constexpr u32 RBitsOut = Bits::popcount(DestFormat::RedMask);
		constexpr u32 GBitsOut = Bits::popcount(DestFormat::GreenMask);
		constexpr u32 BBitsOut = Bits::popcount(DestFormat::BlueMask);
		constexpr u32 ABitsOut = Bits::popcount(DestFormat::AlphaMask);

		SrcType RValueIn = (t & SrcFormat::RedMask) >> SrcFormat::RedShift;
		SrcType GValueIn = (t & SrcFormat::GreenMask) >> SrcFormat::GreenShift;
		SrcType BValueIn = (t & SrcFormat::BlueMask) >> SrcFormat::BlueShift;
		SrcType AValueIn = (t & SrcFormat::AlphaMask) >> SrcFormat::AlphaShift;

		DestType RValueOut = convertChannel<ConvType, RBitsIn, RBitsOut>(RValueIn) << DestFormat::RedShift;
		DestType GValueOut = convertChannel<ConvType, GBitsIn, GBitsOut>(GValueIn) << DestFormat::GreenShift;
		DestType BValueOut = convertChannel<ConvType, BBitsIn, BBitsOut>(BValueIn) << DestFormat::BlueShift;
		DestType AValueOut = convertChannel<ConvType, ABitsIn, ABitsOut>(AValueIn) << DestFormat::AlphaShift;

		AValueOut |= DestFormat::AlphaMask * (ABitsIn == 0);

		DestType out = RValueOut | GValueOut | BValueOut | AValueOut;

		return DestPixelType(out);

	}

	template<Pixel DestPixel, CC::UnsignedIntegral T>
	constexpr static auto convert(T pixel, T redMask, u32 redShift, T greenMask, u32 greenShift, T blueMask, u32 blueShift, T alphaMask = 0, u32 alphaShift = 0) {

		using DestFormat = PixelFormat<DestPixel>;
		using DestPixelType = PixelType<DestPixel>;
		using DestType = typename DestPixelType::PackedT;
		using ConvType = TT::Conditional<(sizeof(T) > sizeof(DestType)), T, DestType>;

		u32 RBitsIn = Bits::popcount(redMask);
		u32 GBitsIn = Bits::popcount(greenMask);
		u32 BBitsIn = Bits::popcount(blueMask);
		u32 ABitsIn = Bits::popcount(alphaMask);
		constexpr u32 RBitsOut = Bits::popcount(DestFormat::RedMask);
		constexpr u32 GBitsOut = Bits::popcount(DestFormat::GreenMask);
		constexpr u32 BBitsOut = Bits::popcount(DestFormat::BlueMask);
		constexpr u32 ABitsOut = Bits::popcount(DestFormat::AlphaMask);

		T RValueIn = (pixel & redMask) >> redShift;
		T GValueIn = (pixel & greenMask) >> greenShift;
		T BValueIn = (pixel & blueMask) >> blueShift;
		T AValueIn = (pixel & alphaMask) >> alphaShift;

		DestType RValueOut = convertChannel<ConvType>(RValueIn, RBitsIn, RBitsOut) << DestFormat::RedShift;
		DestType GValueOut = convertChannel<ConvType>(GValueIn, GBitsIn, GBitsOut) << DestFormat::GreenShift;
		DestType BValueOut = convertChannel<ConvType>(BValueIn, BBitsIn, BBitsOut) << DestFormat::BlueShift;
		DestType AValueOut = convertChannel<ConvType>(AValueIn, ABitsIn, ABitsOut) << DestFormat::AlphaShift;

		DestType out = RValueOut | GValueOut | BValueOut | AValueOut;

		return DestPixelType(out);

	}

};