#pragma once


/**************************************************/
/*			   Arclight Configuration			  */
/**************************************************/

/*
	Defines if this project should use ImGui
*/

#ifndef ARC_USE_IMGUI
#define ARC_USE_IMGUI		0
#endif

/*
	Defines if this project should use the Discord Game SDK
*/

#ifndef ARC_USE_DISCORD
#define ARC_USE_DISCORD		0
#endif

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
	Color quantization function
	quantizeScale8: Scales float to u8
	quantizeRound8: Scales float to u8 and rounds (CMATH_CONSTEXPR issue)
*/

#define ARC_COLOR_QUANTIZE		quantizeScale8


/*
	Default color channel ordering
	ARGB: Alpha highest byte
	RGBA: Alpha lowest byte
*/
#define ARC_COLOR_DEFAULT_CHANNEL_ORDER	ARGB


/*
	Task Executor sleep policy
	ARC_TASK_SPIN: Spin when queue empty
	ARC_TASK_SLEEP_ATOMIC: Atomic wait until new element arrives
	ARC_TASK_PERIODIC_SLEEP: Sleep for a certain amount of time (ARC_TASK_SLEEP_DURATION in us)
*/

#define ARC_TASK_SLEEP_DURATION	50
#define ARC_TASK_SPIN
//#define ARC_TASK_SLEEP_ATOMIC
//#define ARC_TASK_PERIODIC_SLEEP


/*
	Task Executor termination method (destruction only)
	ARC_TASK_END_WAIT: Assists until tasks are finished
*/

#define ARC_TASK_END_WAIT


/*
	Log settings
	ARC_LOG_STDIO_UNSYNC: Unsyncs stdio from cout. Logging is accelerated but data races become possible.
	ARC_LOG_EXCEPTION_ABORT: Aborts when Log throws. Disabled in final build mode.
*/

#define ARC_LOG_STDIO_UNSYNC
#define ARC_LOG_EXCEPTION_ABORT


/*
	Allocator debugging
	ARC_ALLOCATOR_DEBUG_CHECKS: Enables debug assertions during heap creation/allocation
	ARC_ALLOCATOR_DEBUG_LOG: Logs all allocations performed with Arclight Allocators
*/

#define ARC_ALLOCATOR_DEBUG_CHECKS
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
	ACS component storage
	ARC_ACS_FORCE_STACK: Forces the component storage on stack
	ARC_ACS_MAX_COMPONENTS: Maximum component count
*/
#define ARC_ACS_FORCE_STACK


/*
	ACS runtime checks
	ARC_ACS_RUNTIME_CHECKS: Enables safety guards/barriers at runtime
*/
#define ARC_ACS_RUNTIME_CHECKS


/*
	ACS component count
	ARC_ACS_MAX_COMPONENTS: Maximum number of components. Each component ID must be smaller than this.
*/
#define ARC_ACS_MAX_COMPONENTS 256


/*
	Standard float type
	ARC_STD_FLOAT_TYPE: Specifies the underlying standard float type
*/
#define ARC_STD_FLOAT_TYPE float


/*
	Profiling mode
	ARC_ENABLE_PROFILER: Enables profilers
*/
//#define ARC_ENABLE_PROFILER


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
#define ARC_IMAGE_DEBUG