/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 common.hpp
 */

#pragma once

#include "common/config.hpp"
#include "util/preprocessor.hpp"

#include <version>



#define __ARC_MATH_ALIAS(upper, class, name, type, usuffix, lsuffix, ...) \
using ARC_PP_CAT(name, ARC_PP_IF_ELSE(upper, usuffix, lsuffix)) = class<__VA_ARGS__ __VA_OPT__(,) type>;

// Define all integral types aliases of template class
#define ARC_DEFINE_INTEGRAL_ALIASES(upper, class, name, ...) \
	__ARC_MATH_ALIAS(upper, class, name, bool,	B,	b,	__VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, i8,	C,	c,	__VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, i16,	S,	s,	__VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, i32,	I,	i,	__VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, i64,	L,	l,	__VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, u8,	UC,	uc, __VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, u16,	US,	us, __VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, u32,	UI,	ui, __VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, u64,	UL,	ul, __VA_ARGS__)

#define ARC_DEFINE_FLOAT_ALIASES(upper, class, name, ...) \
	__ARC_MATH_ALIAS(upper, class, name, float,			F,	f,	__VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, double,		D,	d,	__VA_ARGS__) \
	__ARC_MATH_ALIAS(upper, class, name, long double,	LD,	ld,	__VA_ARGS__)

#define ARC_DEFINE_ARITHMETIC_ALIASES(upper, class, name, ...) \
	ARC_DEFINE_INTEGRAL_ALIASES(upper, class, name, __VA_ARGS__) \
	ARC_DEFINE_FLOAT_ALIASES(upper, class, name, __VA_ARGS__)


#if		__cpp_lib_constexpr_cmath == 202306L

	#define ARC_CONSTEXPR_CMATH23 constexpr
	#define ARC_CONSTEXPR_CMATH26 constexpr

#elif	__cpp_lib_constexpr_cmath == 202202L

	#define ARC_CONSTEXPR_CMATH23 constexpr
	#define ARC_CONSTEXPR_CMATH26 ARC_FORCE_INLINE

#else

	#define ARC_CONSTEXPR_CMATH23 ARC_FORCE_INLINE
	#define ARC_CONSTEXPR_CMATH26 ARC_FORCE_INLINE

#endif


namespace Math {

	enum class Rounding {
		Unknown,
		TowardZero,
		Nearest,
		Upward,
		Downward
	};

	enum class Category {
		Zero,
		Subnormal,
		Normal,
		Infinity,
		NaN
	};

}
