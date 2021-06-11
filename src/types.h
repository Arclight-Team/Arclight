#pragma once

#include <cstdint>
#include <type_traits>
#include "arcbuild.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef std::byte	Byte;

#if ARC_MACHINE_BITS == 64
	typedef u64 SystemT;
#elif ARC_MACHINE_BITS == 32
	typedef u32 SystemT;
#else
	typedef u32 SystemT;
#endif

typedef uintptr_t	AddressT;
typedef size_t		AlignT;
typedef size_t		SizeT;

template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept Float = std::is_floating_point_v<T>;

template<typename T>
concept Integer = std::is_integral_v<T>;

template<typename T>
concept Unsigned = std::is_unsigned_v<T>;