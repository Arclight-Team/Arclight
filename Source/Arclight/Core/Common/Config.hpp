/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Config.hpp
 */

#pragma once

#include "Build.hpp"



// `ARC_CFG_INTRINSIC_ENABLE_{Arch}_{Feature}` Enables usage of {Feature} for {Arch} targets
#define ARC_CFG_INTRINSIC_ENABLE_X86_SSE
#define ARC_CFG_INTRINSIC_ENABLE_X86_SSE2
#define ARC_CFG_INTRINSIC_ENABLE_X86_SSE3
#define ARC_CFG_INTRINSIC_ENABLE_X86_SSSE3
#define ARC_CFG_INTRINSIC_ENABLE_X86_SSE41
#define ARC_CFG_INTRINSIC_ENABLE_X86_SSE42
#define ARC_CFG_INTRINSIC_ENABLE_X86_AVX
#define ARC_CFG_INTRINSIC_ENABLE_X86_AVX2

// `ARC_CFG_INTRINSIC_STATIC_{Arch}_{Feature}` Disables runtime checking of {Feature} for {Arch} targets
#define ARC_CFG_INTRINSIC_STATIC_X86_SSE
#define ARC_CFG_INTRINSIC_STATIC_X86_SSE2
#define ARC_CFG_INTRINSIC_STATIC_X86_SSE3
#define ARC_CFG_INTRINSIC_STATIC_X86_SSSE3
#define ARC_CFG_INTRINSIC_STATIC_X86_SSE41
#define ARC_CFG_INTRINSIC_STATIC_X86_SSE42

// Toggles all x86 intrinsic usage
#define ARC_CFG_INTRINSIC_X86

// Toggles all ARM intrinsic usage
#define ARC_CFG_INTRINSIC_ARM


// Unsyncs stdio from cout in Log::init (accelerated logging but data races become possible)
#define ARC_CFG_LOG_STDIO_UNSYNC

// Aborts when Log throws (disabled in final build mode)
#define ARC_CFG_LOG_EXCEPTION_ABORT


// Logs all allocations performed with Arclight allocators
#define ARC_CFG_ALLOCATOR_DEBUG


// Packs dense indices and data tightly together (enhances array caching but might marginally bloat a single cache line during iteration)
#define ARC_CFG_SPARSE_PACK


// Enables the Arclight runtime
//#define ARC_CFG_ARCRT_ENABLE

// Silences logging performed by the runtime
//#define ARC_CFG_ARCRT_SILENT

// Disables log file creation in the runtime
//#define ARC_ARCRT_DISABLE_LOG_FILE

// Displays an error message box on fatal errors
//#define ARC_CFG_ARCRT_MESSAGEBOX


// Disables stacktracing library
#define ARC_CFG_DISABLE_STACKTRACES


// Log directory path
#define ARC_CFG_LOG_DIRECTORY "log"


// Builds the program ready for release (disables all debugging and enables few optimizations)
#define ARC_CFG_FINAL_BUILD


#define ARC_ASSERT_POLICY_NEVER 0 // Never asserts
#define ARC_ASSERT_POLICY_DEBUG 1 // Only in debug mode
#define ARC_ASSERT_POLICY_BUILD 2 // Always asserts except in final build mode

// Assertion policy
#define ARC_CFG_ASSERT_POLICY ARC_ASSERT_POLICY_DEBUG

// Disabled assert becomes compiler assumption
#define ARC_CFG_ASSERT_ASSUME

// Generates more verbose compile time assert output
#define ARC_CFG_ASSERT_VERBOSE


// Prefers a branchless conversion algorithm
//#define ARC_CFG_QUATERNION_MATCONV_BRANCHLESS

// Sets w as the last component
//#define ARC_CFG_QUATERNION_XYZW


// Logs image decoding/encoding information
#define ARC_CFG_IMAGE_DEBUG


// Use rounding instead of truncation during conversion. Note that this method is a lot slower.
#define ARC_CFG_PIXEL_EXACT


// Favor precision over speed
#define ARC_CFG_FILTER_EXACT


// Prints argument tree during parsing for debugging
#define ARC_CFG_ARGPARSE_DEBUG
