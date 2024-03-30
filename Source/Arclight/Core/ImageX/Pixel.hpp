/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Pixel.hpp
 */

#pragma once

#include "Common/Assert.hpp"
#include "Util/Bits.hpp"
#include "PixelFormat.hpp"



struct IPixel {};


template<SizeT Bits>
struct PixelBase : public IPixel {

	constexpr static SizeT BitsPerPixel = Bits;
	constexpr static SizeT BytesPerPixel = Bits::ceilPowerOf2(Bits, 8) / 8;

	constexpr PixelBase() noexcept : storage {} {}

	constexpr explicit PixelBase(std::span<u8> data) noexcept {
		arc_assert(data.size() == BytesPerPixel, "Bad Pixel span");
		std::copy(data.begin(), data.end(), storage);
	}

	constexpr std::span<const u8, BytesPerPixel> bytes() const noexcept {
		return storage;
	}

	constexpr std::span<u8, BytesPerPixel> bytes() noexcept {
		return storage;
	}

protected:

	template<CC::Arithmetic T>
	constexpr T assemble(u32 offset, u32 size) const noexcept {
		return BitSpan(storage, offset, size).template read<T>();
	}

	template<CC::Arithmetic T>
	constexpr void disassemble(T t, u32 offset, u32 size) noexcept {
		return BitSpan(storage, offset, size).template write<T>(size, t);
	}

	u8 storage[BytesPerPixel];

};



template<PixelFormat::FormatChannel R, PixelFormat::FormatChannel G, PixelFormat::FormatChannel B, PixelFormat::FormatChannel A>
class PixelRGBA : public PixelBase<R.bits + G.bits + B.bits + A.bits> {

	using Base = PixelBase<R.bits + G.bits + B.bits + A.bits>;

public:

	constexpr static PixelFormat::FormatChannel Red = R;
	constexpr static PixelFormat::FormatChannel Green = G;
	constexpr static PixelFormat::FormatChannel Blue = B;
	constexpr static PixelFormat::FormatChannel Alpha = A;

	using RedChannel 	= decltype(Red);
	using GreenChannel 	= decltype(Red);
	using BlueChannel 	= decltype(Red);
	using AlphaChannel 	= decltype(Red);

	using RedType 		= typename RedChannel	::template PixelDataType<Red.bits>;
	using GreenType 	= typename GreenChannel	::template PixelDataType<Green.bits>;
	using BlueType 		= typename BlueChannel	::template PixelDataType<Blue.bits>;
	using AlphaType 	= typename AlphaChannel	::template PixelDataType<Alpha.bits>;

	constexpr static PixelFormat Format = PixelFormat::create<R, G, B, A>();
	constexpr static u32 ChannelCount = Format.getChannelCount();

	constexpr static PixelFormat::ChannelOrder<R, G, B, A> ChannelOrder{};


	using Base::Base;

	template<CC::Arithmetic... Value>
	constexpr explicit PixelRGBA(Value... value) noexcept {
		setInternal(value...);
	}

	template<CC::Arithmetic... Value>
	constexpr void set(Value... value) noexcept {
		setInternal(value...);
	}

	constexpr void red(RedType value) noexcept requires (Red.exists()) {
		Base::template disassemble<RedType>(value, Red.offset, Red.bits);
	}

	constexpr RedType red() const noexcept requires (Red.exists()) {
		return Base::template assemble<RedType>(Red.offset, Red.bits);
	}

	constexpr void green(GreenType value) noexcept requires (Green.exists()) {
		Base::template disassemble<GreenType>(value, Green.offset, Green.bits);
	}

	constexpr GreenType green() const noexcept requires (Green.exists()) {
		return Base::template assemble<GreenType>(Green.offset, Green.bits);
	}

	constexpr void blue(BlueType value) noexcept requires (Blue.exists()) {
		Base::template disassemble<BlueType>(value, Blue.offset, Blue.bits);
	}

	constexpr BlueType blue() const noexcept requires (Blue.exists()) {
		return Base::template assemble<BlueType>(Blue.offset, Blue.bits);
	}

	constexpr void alpha(AlphaType value) noexcept requires (Alpha.exists()) {
		Base::template disassemble<AlphaType>(value, Alpha.offset, Alpha.bits);
	}

	constexpr AlphaType alpha() const noexcept requires (Alpha.exists()) {
		return Base::template assemble<AlphaType>(Alpha.offset, Alpha.bits);
	}



	constexpr void add(const PixelRGBA& p) noexcept {

		if constexpr (Red.exists()) {
			red(RedChannel::template Traits<Red.bits>::add(red(), p.red()));
		}

		if constexpr (Green.exists()) {
			green(GreenChannel::template Traits<Green.bits>::add(green(), p.green()));
		}

		if constexpr (Blue.exists()) {
			blue(BlueChannel::template Traits<Blue.bits>::add(blue(), p.blue()));
		}

		if constexpr (Alpha.exists()) {
			alpha(AlphaChannel::template Traits<Alpha.bits>::add(alpha(), p.alpha()));
		}

	}

	constexpr void subtract(const PixelRGBA& p) noexcept {

		if constexpr (Red.exists()) {
			red(RedChannel::template Traits<Red.bits>::subtract(red(), p.red()));
		}

		if constexpr (Green.exists()) {
			green(GreenChannel::template Traits<Green.bits>::subtract(green(), p.green()));
		}

		if constexpr (Blue.exists()) {
			blue(BlueChannel::template Traits<Blue.bits>::subtract(blue(), p.blue()));
		}

		if constexpr (Alpha.exists()) {
			alpha(AlphaChannel::template Traits<Alpha.bits>::subtract(alpha(), p.alpha()));
		}

	}

	template<CC::Arithmetic S>
	constexpr void multiply(S s) noexcept {

		if constexpr (Red.exists()) {
			red(RedChannel::template Traits<Red.bits>::multiply(red(), s));
		}

		if constexpr (Green.exists()) {
			green(GreenChannel::template Traits<Green.bits>::multiply(green(), s));
		}

		if constexpr (Blue.exists()) {
			blue(BlueChannel::template Traits<Blue.bits>::multiply(blue(), s));
		}

		if constexpr (Alpha.exists()) {
			alpha(AlphaChannel::template Traits<Alpha.bits>::multiply(alpha(), s));
		}

	}

	template<CC::Arithmetic S>
	constexpr void divide(S s) noexcept {

		if constexpr (Red.exists()) {
			red(RedChannel::template Traits<Red.bits>::divide(red(), s));
		}

		if constexpr (Green.exists()) {
			green(GreenChannel::template Traits<Green.bits>::divide(green(), s));
		}

		if constexpr (Blue.exists()) {
			blue(BlueChannel::template Traits<Blue.bits>::divide(blue(), s));
		}

		if constexpr (Alpha.exists()) {
			alpha(AlphaChannel::template Traits<Alpha.bits>::divide(alpha(), s));
		}

	}

	constexpr PixelRGBA& operator+=(const PixelRGBA& p) noexcept {
		add(p);
		return *this;
	}

	constexpr PixelRGBA& operator-=(const PixelRGBA& p) noexcept {
		subtract(p);
		return *this;
	}

	template<CC::Arithmetic S>
	constexpr PixelRGBA& operator*=(S s) noexcept {
		multiply(s);
		return *this;
	}

	template<CC::Arithmetic S>
	constexpr PixelRGBA& operator/=(S s) noexcept {
		divide(s);
		return *this;
	}

private:

	template<CC::Arithmetic... Value> requires (sizeof...(Value) == ChannelCount)
	constexpr void setInternal(Value... value) noexcept {

		if constexpr (Red.exists()) {
			red(ChannelOrder.template getValue<0>(value...));
		}

		if constexpr (Green.exists()) {
			green(ChannelOrder.template getValue<1>(value...));
		}

		if constexpr (Blue.exists()) {
			blue(ChannelOrder.template getValue<2>(value...));
		}

		if constexpr (Alpha.exists()) {
			alpha(ChannelOrder.template getValue<3>(value...));
		}

	}

};


using PixelRGB8 =	PixelRGBA<	PixelFormat::UnsignedChannel(0, 8),
								PixelFormat::UnsignedChannel(8, 8),
								PixelFormat::UnsignedChannel(16, 8),
								PixelFormat::UnsignedChannel(0, 0)	>;

using PixelBGR8 =	PixelRGBA<	PixelFormat::UnsignedChannel(16, 8),
								PixelFormat::UnsignedChannel(8, 8),
								PixelFormat::UnsignedChannel(0, 8),
								PixelFormat::UnsignedChannel(0, 0)	>;

using PixelRGB565 =	PixelRGBA<	PixelFormat::UnsignedChannel(0, 5),
								PixelFormat::UnsignedChannel(5, 6),
								PixelFormat::UnsignedChannel(11, 5),
								PixelFormat::UnsignedChannel(0, 0)	>;

using PixelRGBAF =	PixelRGBA<	PixelFormat::FloatChannel(0, 32),
								PixelFormat::FloatChannel(32, 32),
								PixelFormat::FloatChannel(64, 32),
								PixelFormat::FloatChannel(96, 32)	>;

using PixelRGBAD =	PixelRGBA<	PixelFormat::FloatChannel(0, 64),
								PixelFormat::FloatChannel(64, 64),
								PixelFormat::FloatChannel(128, 64),
								PixelFormat::FloatChannel(192, 64)	>;



namespace CC {

	template<class T>
	concept PixelType = CC::BaseOf<T, IPixel>;

}


template<CC::PixelType Pixel>
constexpr Pixel operator+(Pixel p, const Pixel& q) {
	p += q;
	return p;
}

template<CC::PixelType Pixel>
constexpr Pixel operator-(Pixel p, const Pixel& q) {
	p -= q;
	return p;
}

template<CC::PixelType Pixel, CC::Arithmetic S>
constexpr Pixel operator*(Pixel p, S s) {
	p *= s;
	return p;
}

template<CC::PixelType Pixel, CC::Arithmetic S>
constexpr Pixel operator*(S s, Pixel p) {
	p *= s;
	return p;
}

template<CC::PixelType Pixel, CC::Arithmetic S>
constexpr Pixel operator/(Pixel p, S s) {
	p /= s;
	return p;
}