/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcconfig.hpp
 */

#pragma once

#include "arcbuild.hpp"



/**************************************************/
/*			   Arclight Configuration			  */
/**************************************************/

/*
	Switches to using constexpr in C math functions
	Affected functions:
		std::isinf
		std::isnan
		std::sin
		std::cos
		std::tan
		std::asin
		std::acos
		std::atan
		std::atan2
		std::fmod
		std::exp
		std::pow
		std::log
		std::log10
		std::sqrt
		std::cbrt
		std::ceil
		std::floor
		std::trunc
		std::round
*/

#define ARC_CMATH_CONSTEXPR_FIX 0


/*
	Vectorized code
	ARC_VECTORIZE_X86: Vectorize for x86 if targeted
*/
//#define ARC_VECTORIZE_X86


/*
	Log settings
	ARC_LOG_STDIO_UNSYNC: Unsyncs stdio from cout. Logging is accelerated but data races become possible.
	ARC_LOG_EXCEPTION_ABORT: Aborts when Log throws. Disabled in final build mode.
*/

#define ARC_LOG_STDIO_UNSYNC
#define ARC_LOG_EXCEPTION_ABORT


/*
	Allocator debugging
	ARC_ALLOCATOR_DEBUG_LOG: Logs all allocations performed with Arclight Allocators
*/

#define ARC_ALLOCATOR_DEBUG_LOG


/*
	Sparse array packing
	ARC_SPARSE_PACK: Packs dense indices and data tightly together. Enhances array caching but might marginally bloat a single cache line during iteration.
*/

//#define ARC_SPARSE_PACK


/*
	String representing the project's configuration type.
*/

#if ARC_DEBUG
	#define ARC_CONFIGURATION_NAME "Debug"
#else
	#define ARC_CONFIGURATION_NAME "Release"
#endif


/*
	Custom ArcRT
	ARC_USE_ARCRT: Uses the custom arclight runtime
*/
#define ARC_USE_ARCRT


/*
	Show message box upon crash
	ARC_SHOW_CRASH_MESSAGEBOX: Displays a message box when the application crashes
*/
#define ARC_SHOW_CRASH_MESSAGEBOX


/*
	Disable stacktraces
	ARC_DISABLE_STACKTRACES: Disables stacktraces
*/
//#define ARC_DISABLE_STACKTRACES


/*
	Log directory path
*/
#define ARC_LOG_DIRECTORY "log"


/*
	Final release build
	ARC_FINAL_BUILD: Builds the program ready for release. Disables debugging functionality and optimizes certain functions.
*/
//#define ARC_FINAL_BUILD


/*
	Assertion policy
	ARC_ASSERT_NEVER: Never asserts.
	ARC_ASSERT_DEBUG: Asserts only in debug mode.
	ARC_ASSERT_BUILD: Always asserts, except in final build mode. (default)
*/
#define ARC_ASSERT_DEBUG


/*
	Standard float type
	ARC_STD_FLOAT_TYPE: Specifies the underlying standard float type
*/
#define ARC_STD_FLOAT_TYPE float


/*
	Matrix -> Quaternion default conversion method
	ARC_QUATERNION_MATCONV_BRANCHLESS: Prefers a branchless conversion algorithm
*/
//#define ARC_QUATERNION_MATCONV_BRANCHLESS


/*
	Quaternion memory layout
	ARC_QUATERNION_XYZW: Sets w to be the last component
*/
//#define ARC_QUATERNION_XYZW


/*
	Image debugging
	ARC_IMAGE_DEBUG: Prints debug output for images
*/
//#define ARC_IMAGE_DEBUG


/*
	Pixel interpolation strategy
	ARC_PIXEL_EXACT: Use rounding instead of truncation during conversion. Note that this method is a lot slower.
*/
//#define ARC_PIXEL_EXACT



/*
	Filter application strategy
	ARC_FILTER_EXACT: Favor precision over speed.
*/
#define ARC_FILTER_EXACT


/*
	Font debugging
	ARC_FONT_DEBUG: Prints debug output for fonts
*/
//#define ARC_FONT_DEBUG


/*
	Font security
	ARC_FONT_SECURE: Enables safety checks against font instruction attacks
*/
#define ARC_FONT_SECURE