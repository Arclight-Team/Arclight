/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bits.hpp
 */

#pragma once

#include "concepts.hpp"
#include "typetraits.hpp"
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

	template<class Dest, class Src>
	constexpr Dest cast(const Src& src) noexcept {
		return std::bit_cast<Dest>(src);
	}

	template<class Dest, class Src>
	inline Dest rcast(Src& src) noexcept {
		return reinterpret_cast<Dest>(src);
	}
	
	template<Integer T>
	constexpr auto rol(T value, i32 bits) noexcept {
		return std::rotl(static_cast<std::make_unsigned_t<T>>(value), bits);
	}

	template<Integer T>
	constexpr auto ror(T value, i32 bits) noexcept {
		return std::rotr(static_cast<std::make_unsigned_t<T>>(value), bits);
	}

	template<Integer T>
	constexpr auto clz(T value) noexcept {
		return std::countl_zero(static_cast<std::make_unsigned_t<T>>(value));
	}

	template<Integer T>
	constexpr auto ctz(T value) noexcept {
		return std::countr_zero(static_cast<std::make_unsigned_t<T>>(value));
	}

	template<Integer T>
	constexpr auto popcount(T value) noexcept {
		return std::popcount(static_cast<std::make_unsigned_t<T>>(value));
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

	template<Integer T>
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

	template<Integer T>
	constexpr auto countLeadingZeros(T value) noexcept {
		return clz(value);
	}

	template<Integer T>
	constexpr auto countTrailingZeros(T value) noexcept {
		return ctz(value);
	}

	template<Integer T>
	constexpr auto countLeadingOnes(T value) noexcept {
		return clz(~value);
	}

	template<Integer T>
	constexpr auto countTrailingOnes(T value) noexcept {
		return ctz(~value);
	}

	template<Integer T>
	constexpr auto countPopulation(T value) noexcept {
		return popcount(value);
	}

	template<Integer I, Integer... J>
	constexpr I assemble(J... js) noexcept requires (TT::IsAllSame<J...> && (TT::SizeofN<0, J...> * sizeof...(J)) == sizeof(I)) {

		SizeT s = TT::SizeofN<0, J...>;
		SizeT n = sizeof...(js) * s;
		I i = 0;
		
		(((i |= static_cast<I>(js) << (n - s)), i >>= s), ...);

		return i;

	}

	template<Integer I, Integer J>
	constexpr I assemble(J* js) noexcept requires (sizeof(I) / sizeof(J) * sizeof(J) == sizeof(I)) {

		I i = 0;
		SizeT shift = 0;

		for(SizeT n = 0; n < sizeof(I) / sizeof(J); n++) {

			i |= static_cast<I>(js[n]) << shift;
			shift += sizeof(J) * 8;

		}

		return i;

	}

	template<Integer I, Integer... J>
	constexpr void disassemble(I i, J&... js) noexcept requires (TT::IsAllSame<J...> && (TT::SizeofN<0, J...> * sizeof...(J)) == sizeof(I)) {
		((js = i & ~static_cast<J>(0), i >>= (TT::SizeofN<0, J...> * 8)), ...);
	}

	template<Integer I, Integer J>
	constexpr void disassemble(I i, J* js) noexcept requires (sizeof(I) / sizeof(J) * sizeof(J) == sizeof(I)) {

		for(SizeT n = 0; n < sizeof(I) / sizeof(J); n++) {

			js[n] = i & ~static_cast<J>(0);
			i >>= sizeof(J) * 8;

		}

	}

	template<class T>
	consteval SizeT bitCount() noexcept {
		return sizeof(T) * 8;
	}

	template<class T>
	inline auto toByteArray(T* t) noexcept {
		return reinterpret_cast<TT::ConditionalConst<ConstType<T>, u8>*>(t);
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