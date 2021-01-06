#pragma once

/*
	OS check
*/
#ifdef _WIN32
	#define ARC_OS_WINDOWS
#elif __linux__
	#define ARC_OS_LINUX
#elif __APPLE__
	#define ARC_OS_MACOS
#else
	#error "Arclight Build Error: Unknown target OS"
	#define ARC_OS_UNKNOWN
#endif


/*
	Platform check
*/
#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64)
	#define ARC_PLATFORM_AMD64
	#define ARC_PLATFORM_X86
	#define ARC_64
#elif defined(_M_IX86) || defined(_X86_) || defined(i386) || defined(__i386) || defined(__i386__)
	#define ARC_PLATFORM_IA32
	#define ARC_PLATFORM_X86
	#define ARC_32
#elif defined(_M_ARM64) || defined(__aarch64__)
	#define ARC_PLATFORM_AARCH64
	#define ARC_PLATFORM_ARM
	#define ARC_64
#elif defined(_M_ARM) || defined(_M_ARMT) || defined(__arm__) || defined(__thumb__)
	#define ARC_PLATFORM_ARM32
	#define ARC_PLATFORM_ARM
	#define ARC_32
#else
	#error "Arclight Build Error: Unknown target platform"
	#define ARC_PLATFORM_UNKNOWN
	#define ARC_BITS_UNKNOWN
#endif


/*
	Compiler check
*/
#if defined(__clang__)
	#define ARC_COMPILER_CLANG
#elif defined(__GNUC__)
	#define ARC_COMPILER_GCC
#elif defined(_MSC_VER)
	#define ARC_COMPILER_MSVC
#else
	#error "Arclight Build Error: Unknown compiler"
	#define ARC_COMPILER_UNKNOWN
#endif