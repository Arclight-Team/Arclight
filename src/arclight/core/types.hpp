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

using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;

using i8    = int8_t;
using i16   = int16_t;
using i32   = int32_t;
using i64   = int64_t;

using imax  = intmax_t;
using umax  = uintmax_t;

using SizeT     = std::size_t;
using AlignT    = SizeT;
using AddressT  = uintptr_t;

#if ARC_MACHINE_BITS == 64
	using SystemT = u64;
#elif ARC_MACHINE_BITS == 32
	using SystemT = u32;
#elif ARC_MACHINE_BITS == 16
	using SystemT = u16;
#else
	using SystemT = u32;
#endif
