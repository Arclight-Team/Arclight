/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Vendor.hpp
 */

#pragma once

#include "Build.hpp"

#include <utility>


// Suppresses function inlinining by the compiler

#ifdef ARC_COMPILER_MSVC
	#define ARC_NO_INLINE [[msvc::noinline]]
#elif defined(ARC_COMPILER_CLANG) || defined(ARC_COMPILER_GCC)
	#define ARC_NO_INLINE [[gnu::noinline]]
#else
	#define ARC_NO_INLINE
#endif


// Forces the compiler to attempt inlining regardless of optimization level

#ifdef ARC_COMPILER_MSVC
	#define ARC_FORCE_INLINE [[msvc::forceinline]] inline
#elif defined(ARC_COMPILER_CLANG) || defined(ARC_COMPILER_GCC)
	#define ARC_FORCE_INLINE [[gnu::always_inline]] inline
#else
	#define ARC_FORCE_INLINE inline
#endif


// Optimization assumption for the compiler defining the condition to be true

#ifdef ARC_COMPILER_MSVC
	#define arc_assume(condition) do { __assume((condition)); } while (false)
#elif defined(ARC_COMPILER_CLANG)
	#define arc_assume(condition) do { __builtin_assume((condition)); } while (false)
#elif defined(ARC_COMPILER_GCC)
	#define arc_assume(condition) do { __attribute__((assume((condition)))); } while (false)
#endif
