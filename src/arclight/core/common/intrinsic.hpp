/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcintrinsic.hpp
 */

#pragma once

#include "build.hpp"
#include "config.hpp"

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



/*
	Code vectorization
*/
#ifdef ARC_PLATFORM_X86

	/*
		x86 code vectorization: SSE and AVX support (except AVX512)
	*/
	#ifdef ARC_CFG_X86_VECTORIZE

		#include <immintrin.h>

		#ifdef ARC_TARGET_HAS_SSE
			#define ARC_VECTORIZE_X86_SSE
		#endif

		#ifdef ARC_TARGET_HAS_SSE2
			#define ARC_VECTORIZE_X86_SSE2
		#endif

		#ifdef ARC_TARGET_HAS_SSE3
			#define ARC_VECTORIZE_X86_SSE3
		#endif

		#ifdef ARC_TARGET_HAS_SSSE3
			#define ARC_VECTORIZE_X86_SSSE3
		#endif

		#ifdef ARC_TARGET_HAS_SSE4_1
			#define ARC_VECTORIZE_X86_SSE4_1
		#endif

		#ifdef ARC_TARGET_HAS_SSE4_2
			#define ARC_VECTORIZE_X86_SSE4_2
		#endif

		#ifdef ARC_TARGET_HAS_AVX
			#define ARC_VECTORIZE_X86_AVX
		#endif

		#ifdef ARC_TARGET_HAS_AVX2
			#define ARC_VECTORIZE_X86_AVX2
		#endif

	#endif

#endif
