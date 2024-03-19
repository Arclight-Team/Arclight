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



#ifdef ARC_PLATFORM_X86

	// Code vectorization for x86 (SSE and AVX except AVX512)
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
