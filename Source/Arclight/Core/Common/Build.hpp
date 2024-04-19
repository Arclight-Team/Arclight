/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Build.hpp
 */

#pragma once



#if ARC_DEBUG
	#define ARC_BUILD_TYPE "Debug"
#else
	#define ARC_BUILD_TYPE "Release"
#endif


#ifdef _WIN32
	#define ARC_OS_WINDOWS
#elif __linux__
	#define ARC_OS_UNIX
	#define ARC_OS_LINUX
#elif __APPLE__
	#define ARC_OS_UNIX
	#define ARC_OS_MACOS
#else
	#error "Arclight Fatal: Unknown or unsupported target OS"
#endif


#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64)
	#define ARC_PLATFORM_AMD64
	#define ARC_PLATFORM_X86
	#define ARC_MACHINE_BITS 64
#elif defined(_M_IX86) || defined(_X86_) || defined(i386) || defined(__i386) || defined(__i386__)
	#define ARC_PLATFORM_IA32
	#define ARC_PLATFORM_X86
	#define ARC_MACHINE_BITS 32
#elif defined(_M_ARM64) || defined(__aarch64__)
	#define ARC_PLATFORM_AARCH64
	#define ARC_PLATFORM_ARM
	#define ARC_MACHINE_BITS 64
#elif defined(_M_ARM) || defined(_M_ARMT) || defined(__arm__) || defined(__thumb__)
	#define ARC_PLATFORM_ARM32
	#define ARC_PLATFORM_ARM
	#define ARC_MACHINE_BITS 32
#else
	#error "Arclight Fatal: Unknown or unsupported target platform"
#endif


#if defined(__clang__)
	#define ARC_COMPILER_CLANG
	#define ARC_COMPILER_GCCLIKE
#elif defined(__GNUC__)
	#define ARC_COMPILER_GCC
	#define ARC_COMPILER_GCCLIKE
#elif defined(_MSC_VER)
	#define ARC_COMPILER_MSVC
#else
	#error "Arclight Fatal: Unknown or unsupported compiler"
#endif


#define ARC_MACHINE_BYTES (ARC_MACHINE_BITS / 8)
