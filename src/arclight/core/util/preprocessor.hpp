/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 preprocessor.hpp
 */

#pragma once



#define ARC_PP_CATX(a, b)		a##b

#define ARC_PP_CAT(a, b)		ARC_PP_CATX(a, b)
#define ARC_PP_CAT3(a, b, c)	ARC_PP_CAT(ARC_PP_CAT(a, b), c)
#define ARC_PP_CAT4(a, b, c, d)	ARC_PP_CAT(ARC_PP_CAT3(a, b, c), d)


#define ARC_PP_STRX(x) #x

#define ARC_PP_STR(x) ARC_PP_STRX(x)


#define ARC_PP_GET0(m, ...) m
#define ARC_PP_GET1(_1, m, ...) m
#define ARC_PP_GET2(_1, _2, m, ...) m
#define ARC_PP_GET3(_1, _2, _3, m, ...) m
#define ARC_PP_GET4(_1, _2, _3, _4, m, ...) m
#define ARC_PP_GET5(_1, _2, _3, _4, _5, m, ...) m
#define ARC_PP_GET6(_1, _2, _3, _4, _5, _6, m, ...) m
#define ARC_PP_GET7(_1, _2, _3, _4, _5, _6, _7, m, ...) m
#define ARC_PP_GET8(_1, _2, _3, _4, _5, _6, _7, _8, m, ...) m
#define ARC_PP_GET9(_1, _2, _3, _4, _5, _6, _7, _8, _9, m, ...) m


#define ARC_PP_EMPTY
#define ARC_PP_IGNORE(...)
#define ARC_PP_EXPAND(...) __VA_ARGS__


#define ARC_PP_PRAGMA(p) _Pragma(ARC_PP_STR(p))


#define __ARC_BOOL_NEGATE_0 1
#define __ARC_BOOL_NEGATE_1 0

#define __ARC_BOOL_AND_00 0
#define __ARC_BOOL_AND_01 0
#define __ARC_BOOL_AND_10 0
#define __ARC_BOOL_AND_11 1

#define __ARC_BOOL_OR_00 0
#define __ARC_BOOL_OR_01 1
#define __ARC_BOOL_OR_10 1
#define __ARC_BOOL_OR_11 1

#define ARC_PP_BOOL_NEGATE(a)	ARC_PP_CAT(__ARC_BOOL_NEGATE_, a)
#define ARC_PP_BOOL_AND(a, b)	ARC_PP_CAT3(__ARC_BOOL_AND_, a, b)
#define ARC_PP_BOOL_OR(a, b)	ARC_PP_CAT3(__ARC_BOOL_OR_, a, b)

#define ARC_PP_TRUE 1
#define ARC_PP_FALSE 0


#define __ARC_TERNARY_1(t, f) t
#define __ARC_TERNARY_0(t, f) f

#define ARC_PP_IF_ELSE(c, t, f)	ARC_PP_CAT(__ARC_TERNARY_, c)(t, f)
#define ARC_PP_IF(c, t)			ARC_PP_CAT(__ARC_TERNARY_, c)(t, )


#define ARC_PP_FILE			__FILE__
#define ARC_PP_LINE			__LINE__
#define ARC_PP_DATE			__DATE__
#define ARC_PP_TIME			__TIME__
#define ARC_PP_COUNTER		__COUNTER__
