/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 types.hpp
 */

#pragma once

#include "arcbuild.hpp"

#include <cstdint>
#include <cstddef>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef intmax_t	imax;
typedef uintmax_t	umax;

typedef u8 Byte;

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

constexpr inline SizeT PointerSize = sizeof(void*);
constexpr inline AlignT PointerAlign = alignof(void*);

template<class T>
struct TypeTag {};