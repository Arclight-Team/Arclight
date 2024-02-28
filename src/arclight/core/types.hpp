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



using u8		= std::uint8_t;
using u16		= std::uint16_t;
using u32		= std::uint32_t;
using u64		= std::uint64_t;

using i8		= std::int8_t;
using i16		= std::int16_t;
using i32		= std::int32_t;
using i64		= std::int64_t;

using imax		= std::intmax_t;
using umax		= std::uintmax_t;

using SizeT		= std::size_t;
using AlignT	= SizeT;
using AddressT	= std::uintptr_t;
using NullptrT	= std::nullptr_t;

#if ARC_MACHINE_BITS == 64
	using SystemT = u64;
#elif ARC_MACHINE_BITS == 32
	using SystemT = u32;
#elif ARC_MACHINE_BITS == 16
	using SystemT = u16;
#else
	using SystemT = u32;
#endif


#define __ARC_DEFINE_INTEGRAL_LITERAL(type, suffix) \
constexpr type operator""_##suffix(unsigned long long n) { \
	return type(n); \
}

__ARC_DEFINE_INTEGRAL_LITERAL(u8, u8)
__ARC_DEFINE_INTEGRAL_LITERAL(u16, u16)
__ARC_DEFINE_INTEGRAL_LITERAL(u32, u32)
__ARC_DEFINE_INTEGRAL_LITERAL(u64, u64)

__ARC_DEFINE_INTEGRAL_LITERAL(i8, i8)
__ARC_DEFINE_INTEGRAL_LITERAL(i16, i16)
__ARC_DEFINE_INTEGRAL_LITERAL(i32, i32)
__ARC_DEFINE_INTEGRAL_LITERAL(i64, i64)

#undef __ARC_DEFINE_INTEGRAL_LITERAL
