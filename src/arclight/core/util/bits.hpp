/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bits.hpp
 */

#pragma once

#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "types.hpp"

#include <bit>



enum class ByteOrder {
	Little,
	Big
};

constexpr inline ByteOrder MachineByteOrder = std::endian::native == std::endian::little ? ByteOrder::Little : ByteOrder::Big;
constexpr inline bool LittleEndian			= MachineByteOrder == ByteOrder::Little;
constexpr inline bool BigEndian				= MachineByteOrder == ByteOrder::Big;

namespace Bits {

	constexpr u8 reverseLookupTable[] = {
		0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0,
		0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
		0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
		0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
		0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4,
		0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
		0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
		0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
		0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
		0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
		0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA,
		0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
		0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6,
		0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
		0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
		0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
		0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1,
		0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
		0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9,
		0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
		0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
		0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
		0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED,
		0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
		0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3,
		0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
		0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
		0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
		0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7,
		0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
		0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF,
		0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
	};

	template<class Dest, class Src>
	constexpr Dest cast(const Src& src) noexcept {
		return std::bit_cast<Dest>(src);
	}

	template<class Dest, class Src>
	inline Dest rcast(Src& src) noexcept {
		return reinterpret_cast<Dest>(src);
	}

	template<class Dest, class Src>
	inline Dest rcast(Src* src) noexcept {
		return reinterpret_cast<Dest>(src);
	}

	template<CC::Integer T>
	constexpr auto rol(T value, i32 bits) noexcept {
		return std::rotl(static_cast<TT::MakeUnsigned<T>>(value), bits);
	}

	template<CC::Integer T>
	constexpr auto ror(T value, i32 bits) noexcept {
		return std::rotr(static_cast<TT::MakeUnsigned<T>>(value), bits);
	}

	template<CC::Integer T>
	constexpr auto clz(T value) noexcept {
		return std::countl_zero(static_cast<TT::MakeUnsigned<T>>(value));
	}

	template<CC::Integer T>
	constexpr auto ctz(T value) noexcept {
		return std::countr_zero(static_cast<TT::MakeUnsigned<T>>(value));
	}

	template<CC::UnsignedType T>
	constexpr bool isPowerOf2(T value) noexcept {
		return std::has_single_bit(value);
	}

	template<CC::UnsignedType T>
	constexpr T ceilPowerOf2(T value) noexcept {
		return std::bit_ceil(value);
	}

	template<CC::UnsignedType T>
	constexpr T floorPowerOf2(T value) noexcept {
		return std::bit_floor(value);
	}

	template<CC::Integer T>
	constexpr auto popcount(T value) noexcept {
		return std::popcount(static_cast<TT::MakeUnsigned<T>>(value));
	}

	constexpr auto swap16(u16 in) noexcept -> decltype(in) {
		return ((in & 0xFF) << 8) | ((in) >> 8);
	}

	constexpr auto swap32(u32 in) noexcept -> decltype(in) {
		return (swap16(in & 0xFFFF) << 16) | (swap16(in >> 16));
	}

	constexpr auto swap64(u64 in) noexcept -> decltype(in) {
		return (u64(swap32(in & 0xFFFFFFFF)) << 32) | (swap32(in >> 32));
	}

	template<CC::Integer T>
	constexpr T swap(T in) noexcept {

		if constexpr (sizeof(T) == 8) {
			return Bits::cast<T>(swap64(Bits::cast<u64>(in)));
		} else if constexpr (sizeof(T) == 4) {
			return Bits::cast<T>(swap32(Bits::cast<u32>(in)));
		} else if constexpr (sizeof(T) == 2) {
			return Bits::cast<T>(swap16(Bits::cast<u16>(in)));
		} else if constexpr (sizeof(T) == 1) {
			return in;
		} else {
			static_assert("Illegal byte swap");
		}

	}

	template<CC::Integer T>
	constexpr T reverse(T in) noexcept {

		auto t = Bits::cast<TT::MakeUnsigned<T>>(in);

		if constexpr (sizeof(T) == 8) {
			return Bits::cast<T>(reverse<u32>(t >> 32) | (u64(reverse<u32>(t & 0xFFFFFFFF)) << 32));
		} else if constexpr (sizeof(T) == 4) {
			return Bits::cast<T>(static_cast<u32>(reverse<u16>(t >> 16) | (reverse<u16>(t & 0xFFFF) << 16)));
		} else if constexpr (sizeof(T) == 2) {
			return Bits::cast<T>(static_cast<u16>(reverse<u8>(t >> 8) | (reverse<u8>(t & 0xFF) << 8)));
		} else if constexpr (sizeof(T) == 1) {
			return reverseLookupTable[t];
		} else {
			static_assert("Illegal reverse");
		}

	}

	constexpr auto big16(u16 in) noexcept {
		if constexpr (BigEndian) {
			return in;
		} else {
			return swap16(in);
		}
	}

	constexpr auto big32(u32 in) noexcept {
		if constexpr (BigEndian) {
			return in;
		}
		else {
			return swap32(in);
		}
	}

	constexpr auto big64(u64 in) noexcept {
		if constexpr (BigEndian) {
			return in;
		}
		else {
			return swap64(in);
		}
	}

	constexpr auto little16(u16 in) noexcept {
		if constexpr (LittleEndian) {
			return in;
		}
		else {
			return swap16(in);
		}
	}

	constexpr auto little32(u32 in) noexcept {
		if constexpr (LittleEndian) {
			return in;
		}
		else {
			return swap32(in);
		}
	}

	constexpr auto little64(u64 in) noexcept {
		if constexpr (LittleEndian) {
			return in;
		}
		else {
			return swap64(in);
		}
	}

	template<class T>
	constexpr T big(T in) noexcept {

		if constexpr (sizeof(T) == 8) {
			return Bits::cast<T>(big64(Bits::cast<u64>(in)));
		} else if constexpr (sizeof(T) == 4) {
			return Bits::cast<T>(big32(Bits::cast<u32>(in)));
		} else if constexpr (sizeof(T) == 2) {
			return Bits::cast<T>(big16(Bits::cast<u16>(in)));
		} else if constexpr (sizeof(T) == 1) {
			return in;
		} else {
			static_assert("Illegal byte swap");
		}

	}

	template<class T>
	constexpr T little(T in) noexcept {

		if constexpr (sizeof(T) == 8) {
			return Bits::cast<T>(little64(Bits::cast<u64>(in)));
		} else if constexpr (sizeof(T) == 4) {
			return Bits::cast<T>(little32(Bits::cast<u32>(in)));
		} else if constexpr (sizeof(T) == 2) {
			return Bits::cast<T>(little16(Bits::cast<u16>(in)));
		} else if constexpr (sizeof(T) == 1) {
			return in;
		} else {
			static_assert("Illegal byte swap");
		}

	}

	constexpr bool requiresEndianConversion(ByteOrder reqOrder) noexcept {
		return reqOrder != MachineByteOrder;
	}

	template<CC::Integer T>
	constexpr auto countLeadingZeros(T value) noexcept {
		return clz(value);
	}

	template<CC::Integer T>
	constexpr auto countTrailingZeros(T value) noexcept {
		return ctz(value);
	}

	template<CC::Integer T>
	constexpr auto countLeadingOnes(T value) noexcept {
		return clz(~value);
	}

	template<CC::Integer T>
	constexpr auto countTrailingOnes(T value) noexcept {
		return ctz(~value);
	}

	template<CC::Integer T>
	constexpr auto countPopulation(T value) noexcept {
		return popcount(value);
	}

	template<CC::Integer I, CC::Integer... J>
	constexpr I assemble(J... js) noexcept requires (TT::IsAllSame<J...> && (TT::SizeofN<0, J...> * sizeof...(J)) == sizeof(I)) {

		SizeT s = TT::SizeofN<0, J...>;
		SizeT n = sizeof...(js) * s;
		SizeT shift = 0;
		I i = 0;

		(((i |= static_cast<TT::MakeUnsigned<I>>(cast<TT::MakeUnsigned<J>>(js)) << shift), shift += s * 8), ...);

		return i;

	}

	template<CC::Integer I, CC::Integer J>
	constexpr I assemble(J* js, SizeT max = -1) noexcept requires (sizeof(I) / sizeof(J) * sizeof(J) == sizeof(I)) {

		I i = 0;
		SizeT shift = 0;

		for (SizeT n = 0; n < sizeof(I) / sizeof(J) && n < max; n++) {

			i |= static_cast<TT::MakeUnsigned<I>>(cast<TT::MakeUnsigned<J>>(js[n])) << shift;
			shift += sizeof(J) * 8;

		}

		return i;

	}

	template<CC::Integer I, CC::Integer... J>
	constexpr void disassemble(I i, J&... js) noexcept requires (TT::IsAllSame<J...> && (TT::SizeofN<0, J...> * sizeof...(J)) == sizeof(I)) {
		((js = cast<J>(TT::MakeUnsigned<J>(i & ~static_cast<J>(0))), i >>= (TT::SizeofN<0, J...> * 8)), ...);
	}

	template<CC::Integer I, CC::Integer J>
	constexpr void disassemble(I i, J* js, SizeT max = -1) noexcept requires (sizeof(I) / sizeof(J) * sizeof(J) == sizeof(I)) {

		for(SizeT n = 0; n < sizeof(I) / sizeof(J) && n < max; n++) {

			js[n] = cast<J>(TT::MakeUnsigned<J>(i & ~static_cast<J>(0)));
			i >>= sizeof(J) * 8;

		}

	}

	template<class T>
	constexpr SizeT bitCount() noexcept {
		return sizeof(T) * 8;
	}

	template<CC::Integer T>
	constexpr T mask(T t, u32 start, u32 count) noexcept {
		return t & (((T(1) << count) - 1) << start);
	}

	template<CC::Integer T>
	constexpr T clear(T t, u32 start, u32 count) noexcept {
		return t & ~(((T(1) << count) - 1) << start);
	}

	template<class T>
	inline auto toByteArray(T* t) noexcept {
		return reinterpret_cast<TT::ConditionalConst<CC::ConstType<T>, u8>*>(t);
	}

}


#define arc_clz(x) Bits::clz((x))
#define arc_ctz(x) Bits::ctz((x))

#define arc_countpop(x) Bits::popcount((x))
#define arc_popcount(x) Bits::popcount((x))

#define arc_ror(x, b) Bits::ror((x), (b))
#define arc_rol(x, b) Bits::rol((x), (b))

#define arc_swap16(x) Bits::swap16((x))
#define arc_swap32(x) Bits::swap32((x))
#define arc_swap64(x) Bits::swap64((x))

#define arc_big16(x) Bits::big16((x))
#define arc_big32(x) Bits::big32((x))
#define arc_big64(x) Bits::big64((x))

#define arc_little16(x) Bits::little16((x))
#define arc_little32(x) Bits::little32((x))
#define arc_little64(x) Bits::little64((x))