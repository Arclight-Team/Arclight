#pragma once

#include "arcconfig.h"
#include "util/log.h"
	
#define arc_abort()						do { std::abort(); } while (false)
#define arc_exit(x)						do { std::exit((x)); } while(false)


template<class... Args>
constexpr void __arc_assert(bool condition, const std::string& file, int line, const std::string& message, const Args&... args) noexcept {

	if (!condition) {
		Log::error("Assert", "Assertion failed (File " + file + ", line " + std::to_string(line) + "): " + message, args...);
		arc_abort();
	}

}


#if defined(ARC_FINAL_BUILD) || (defined(ARC_ASSERT_DEBUG) && defined(NDEBUG)) || defined(ARC_ASSERT_NEVER)
	#define arc_assert(cond, msg, ...)
	#define arc_force_assert(msg, ...)
#else
	#define arc_assert(cond, msg, ...)	do {__arc_assert(!!(cond), __FILE__, __LINE__, (msg), ##__VA_ARGS__);} while (false)
	#define arc_force_assert(msg, ...)	do {__arc_assert(false, __FILE__, __LINE__, (msg), ##__VA_ARGS__);} while (false)
#endif