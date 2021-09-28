#pragma once

#include <bit>
#include "util/concepts.h"
#include "types.h"


enum class ByteOrder {
	Little,
	Big
};

constexpr inline ByteOrder MachineByteOrder = std::endian::native == std::endian::little ? ByteOrder::Little : ByteOrder::Big;
constexpr inline bool LittleEndian			= MachineByteOrder == ByteOrder::Little;
constexpr inline bool BigEndian				= MachineByteOrder == ByteOrder::Big;

namespace Bits {
	
	template<Integer T>
	constexpr auto rol(T value, int bits) {
		return std::rotl(static_cast<std::make_unsigned_t<T>>(value), bits);
	}

	template<Integer T>
	constexpr auto ror(T value, int bits) {
		return std::rotr(static_cast<std::make_unsigned_t<T>>(value), bits);
	}

	template<Integer T>
	constexpr auto clz(T value) {
		return std::countl_zero(static_cast<std::make_unsigned_t<T>>(value));
	}

	template<Integer T>
	constexpr auto ctz(T value) {
		return std::countr_zero(static_cast<std::make_unsigned_t<T>>(value));
	}

	template<Integer T>
	constexpr auto popcount(T value) {
		return std::popcount(static_cast<std::make_unsigned_t<T>>(value));
	}

	constexpr auto swap16(u16 in) -> decltype(in) {
		return ((in & 0xFF) << 8) | ((in) >> 8);
	}

	constexpr auto swap32(u32 in) -> decltype(in) {
		return (swap16(in & 0xFFFF) << 16) | (swap16(in >> 16));
	}

	constexpr auto swap64(u64 in) -> decltype(in) {
		return (u64(swap32(in & 0xFFFFFFFF)) << 32) | (swap32(in >> 32));
	}

	template<Integer T>
	constexpr T swap(T in) {

		if constexpr (sizeof(T) == 8) {
			return swap64(in);
		} else if constexpr (sizeof(T) == 4) {
			return swap32(in);
		} else if constexpr (sizeof(T) == 2) {
			return swap16(in);
		}

		return in;

	}

	constexpr auto big16(u16 in) {
		if constexpr (BigEndian) {
			return in;
		} else {
			return swap16(in);
		}
	}

	constexpr auto big32(u32 in) {
		if constexpr (BigEndian) {
			return in;
		}
		else {
			return swap32(in);
		}
	}

	constexpr auto big64(u64 in) {
		if constexpr (BigEndian) {
			return in;
		}
		else {
			return swap64(in);
		}
	}

	constexpr auto little16(u16 in) {
		if constexpr (LittleEndian) {
			return in;
		}
		else {
			return swap16(in);
		}
	}

	constexpr auto little32(u32 in) {
		if constexpr (LittleEndian) {
			return in;
		}
		else {
			return swap32(in);
		}
	}

	constexpr auto little64(u64 in) {
		if constexpr (LittleEndian) {
			return in;
		}
		else {
			return swap64(in);
		}
	}

	constexpr bool requiresEndianConversion(ByteOrder reqOrder) {
		return reqOrder != MachineByteOrder;
	}

	template<Integer T>
	constexpr auto countLeadingZeros(T value) {
		return clz(value);
	}

	template<Integer T>
	constexpr auto countTrailingZeros(T value) {
		return ctz(value);
	}

	template<Integer T>
	constexpr auto countLeadingOnes(T value) {
		return clz(~value);
	}

	template<Integer T>
	constexpr auto countTrailingOnes(T value) {
		return ctz(~value);
	}

	template<Integer T>
	constexpr auto countPopulation(T value) {
		return popcount(value);
	}

}


template<class O, class I>
constexpr O ptr_cast(I* v) {
	return static_cast<O>(static_cast<void*>(v));
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