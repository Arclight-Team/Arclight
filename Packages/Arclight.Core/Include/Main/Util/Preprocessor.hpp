/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Preprocessor.hpp
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


#define ARC_PP_EVAL0(...) __VA_ARGS__
#define ARC_PP_EVAL1(...) ARC_PP_EVAL0(ARC_PP_EVAL0(ARC_PP_EVAL0(__VA_ARGS__)))
#define ARC_PP_EVAL2(...) ARC_PP_EVAL1(ARC_PP_EVAL1(ARC_PP_EVAL1(__VA_ARGS__)))
#define ARC_PP_EVAL3(...) ARC_PP_EVAL2(ARC_PP_EVAL2(ARC_PP_EVAL2(__VA_ARGS__)))
#define ARC_PP_EVAL4(...) ARC_PP_EVAL3(ARC_PP_EVAL3(ARC_PP_EVAL3(__VA_ARGS__)))
#define ARC_PP_EVAL5(...) ARC_PP_EVAL4(ARC_PP_EVAL4(ARC_PP_EVAL4(__VA_ARGS__)))

#define ARC_PP_EVAL(...) ARC_PP_EVAL5(__VA_ARGS__)

#define ARC_PP_EVAL_LIMIT 365


#define ARC_PP_EMPTY
#define ARC_PP_COMMA ,
#define ARC_PP_IGNORE(...)
#define ARC_PP_EXPAND(...) __VA_ARGS__

#define ARC_PP_DELAY(m) m ARC_PP_EMPTY


#define __ARC_IS_NULL_0	0
#define __ARC_IS_NULL	1

#define __ARC_IS_NULL_CHECK(...) ARC_PP_CATX(__ARC_IS_NULL, __VA_OPT__(_0))

#define ARC_PP_IS_NULL(m) __ARC_IS_NULL_CHECK(m)


#define __ARC_FOREACH_0(m, x, ...) m(x) __VA_OPT__(ARC_PP_DELAY(__ARC_FOREACH_1)(m, __VA_ARGS__))
#define __ARC_FOREACH_1(m, x, ...) m(x) __VA_OPT__(ARC_PP_DELAY(__ARC_FOREACH_0)(m, __VA_ARGS__))

#define ARC_PP_FOREACH(m, ...) ARC_PP_EVAL(__ARC_FOREACH_1(m, __VA_ARGS__))


#define ARC_PP_PRAGMA(p) _Pragma(ARC_PP_STR(p))


#define __ARC_BOOL_NOT_0 1
#define __ARC_BOOL_NOT_1 0

#define __ARC_BOOL_AND_00 0
#define __ARC_BOOL_AND_01 0
#define __ARC_BOOL_AND_10 0
#define __ARC_BOOL_AND_11 1

#define __ARC_BOOL_OR_00 0
#define __ARC_BOOL_OR_01 1
#define __ARC_BOOL_OR_10 1
#define __ARC_BOOL_OR_11 1

#define ARC_PP_BOOL_NOT(a)		ARC_PP_CAT(__ARC_BOOL_NOT_, a)
#define ARC_PP_BOOL_AND(a, b)	ARC_PP_CAT3(__ARC_BOOL_AND_, a, b)
#define ARC_PP_BOOL_OR(a, b)	ARC_PP_CAT3(__ARC_BOOL_OR_, a, b)

#define ARC_PP_TRUE 1
#define ARC_PP_FALSE 0


#define __ARC_TERNARY_1(t, f) t
#define __ARC_TERNARY_0(t, f) f

#define ARC_PP_IF_ELSE(c, t, f)	ARC_PP_CAT(__ARC_TERNARY_, c)(t, f)
#define ARC_PP_IF(c, t)			ARC_PP_CAT(__ARC_TERNARY_, c)(t, ARC_PP_EMPTY)


#define ARC_PP_FILE	__FILE__
#define ARC_PP_LINE	__LINE__
#define ARC_PP_DATE	__DATE__
#define ARC_PP_TIME	__TIME__
