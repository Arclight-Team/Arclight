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

#include <utility>
#include <source_location>



#define arc_abort()		do { std::abort(); } while (false)
#define arc_exit(code)	do { std::exit((code)); } while (false)


template<class... Args>
constexpr void __arc_assert(bool condition, const std::source_location& location, const std::string& message, Args&&... args) noexcept {

	if (condition) {

		return;

	} else if (std::is_constant_evaluated()) {

		std::unreachable(); // Simplify constant evaluation failure

	} else {

		try {

			LogE("Assert").print("Assertion failed in file '%s' [%d:%d] in function '%s':", location.file_name(), location.line(), location.column(), location.function_name());
			LogE("Assert").print("\t" + message, std::forward<Args>(args)...);

		} catch (...) {

			LogE("Assert") << "Fatal error: Failed to assert";

		}

		arc_abort();

	}

}


// TODO [[assume(cond)]] in release

#if defined(ARC_FINAL_BUILD) || (defined(ARC_ASSERT_DEBUG) && ARC_RELEASE) || defined(ARC_ASSERT_NEVER)

	#define arc_assert(cond, msg, ...) do {} while (false)
	#define arc_force_assert(msg, ...) do {} while (false)

#else

	#define arc_assert(cond, msg, ...) do { __arc_assert(!!(cond), std::source_location::current(), msg __VA_OPT__(,) __VA_ARGS__); } while (false)
	#define arc_force_assert(msg, ...) arc_assert(false, msg __VA_OPT__(,) __VA_ARGS__)

#endif
