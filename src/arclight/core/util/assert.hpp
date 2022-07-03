/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 assert.hpp
 */

#pragma once

#include "log.hpp"
#include "types.hpp"
#include "arcconfig.hpp"

#include <source_location>

#define arc_abort()						do { std::abort(); } while (false)
#define arc_exit(x)						do { std::exit((x)); } while(false)


template<class... Args>
constexpr void __arc_assert(bool condition, const std::source_location& srcLocation, const std::string& message, Args&&... args) noexcept {

	if (!condition) {

		try {

			Log::error("Assert", "Assertion failed in file '" + std::string(srcLocation.file_name()) + "' [" + std::to_string(srcLocation.line()) + ":" + std::to_string(srcLocation.column()) + "] in function '" + std::string(srcLocation.function_name()) + "':");
			Log::error("Assert", "\t" + message, std::forward<Args>(args)...);

		} catch(...) {

			Log::error("Assert", "Fatal error: Failed to assert");

		}

		arc_abort();

	}

}


#if defined(ARC_FINAL_BUILD) || (defined(ARC_ASSERT_DEBUG) && ARC_RELEASE) || defined(ARC_ASSERT_NEVER)
	#define arc_assert(cond, msg, ...)  do {} while (false)
	#define arc_force_assert(msg, ...)  do {} while (false)
#else
	#define arc_assert(cond, msg, ...)	do {__arc_assert(!!(cond), std::source_location::current(), (msg), ##__VA_ARGS__);} while (false)
	#define arc_force_assert(msg, ...)	arc_assert(false, msg, __VA_ARGS__)
#endif