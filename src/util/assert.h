#pragma once

#include "util/log.h"


template<class... Args>
void __arc_assert(bool condition, const std::string& file, int line, const std::string& message, const Args&... args) {

	if (!condition) {
		Log::error("Assert", "Assertion failed (File " + file + ", line " + std::to_string(line) + "): " + message, args...);
		std::abort();
	}

}


#ifndef NDEBUG
	#define arc_assert(cond, msg, ...)	do {__arc_assert((cond), __FILE__, __LINE__, (msg), ##__VA_ARGS__);} while (false)
	#define arc_force_assert(msg, ...)	do {__arc_assert(false, __FILE__, __LINE__, (msg), ##__VA_ARGS__);} while (false)
#else
	#define arc_assert(cond, msg, ...)
	#define arc_force_assert(msg, ...)
#endif
	
#define arc_abort()						do { std::abort(); } while (false)