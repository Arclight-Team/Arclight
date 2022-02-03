/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcintrinsic.hpp
 */

#pragma once

#include "arcbuild.hpp"
#include "arcconfig.hpp"

/*
	Vectorization includes
*/
#if defined(ARC_PLATFORM_X86)
	#include <immintrin.h>
#endif



/*
	Defines ARC_FORCE_INLINE
	Forces the compiler to inline a certain function.
*/
#ifdef ARC_COMPILER_MSVC
	#define ARC_FORCE_INLINE	__forceinline
#elif defined(ARC_COMPILER_CLANG) || defined(ARC_COMPILER_GCC)
	#define ARC_FORCE_INLINE	__attribute__((always_inline)) inline
#else
	#define ARC_FORCE_INLINE	inline
#endif



/*
	Defines ARC_UNREACHABLE
	Issues a hint to the compiler that the given codepath is never reachable,
*/
#ifdef ARC_COMPILER_MSVC
	#define ARC_UNREACHABLE	__assume(false);
#elif defined(ARC_COMPILER_CLANG) || defined(ARC_COMPILER_GCC)
	#define ARC_UNREACHABLE	__builtin_unreachable();
#else
	#define ARC_UNREACHABLE
#endif



/*
	Code vectorization
*/
#ifdef ARC_PLATFORM_X86

	/*
		x86 code vectorization: SSE and AVX support (except AVX512)
	*/
	#ifdef ARC_VECTORIZE_X86

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