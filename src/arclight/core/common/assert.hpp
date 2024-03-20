// Copyright (c) 2021-2024 - Arclight Team
//
// This file is part of Arclight. All rights reserved.
//
// assert.hpp

#pragma once

#include "util/log.hpp"
#include "common/config.hpp"
#include "common/vendor.hpp"

#include <utility>
#include <source_location>



#define arc_abort()		do { std::abort(); } while (false)
#define arc_exit(code)	do { std::exit((code)); } while (false)


#ifdef ARC_CFG_ASSERT_VERBOSE
	#define __ARC_ASSERT_CONSTEXPR_FAIL __It_could_be_that_perchance_the_Arclight_assertion_mechanism_had_a_negative_outcome_during_compile_time_evaluation
#else
	#define __ARC_ASSERT_CONSTEXPR_FAIL __arc_assert_failed_at_compile_time
#endif

inline void __ARC_ASSERT_CONSTEXPR_FAIL() {}

static constexpr void __arc_assert_constexpr(bool condition) noexcept {

	if (std::is_constant_evaluated() && !condition) {
		__ARC_ASSERT_CONSTEXPR_FAIL();
	}

}

#undef __ARC_ASSERT_CONSTEXPR_FAIL


template<class... Args>
static constexpr void __arc_assert(bool condition, const std::source_location& location, const std::string& message, Args&&... args) noexcept {

	if (condition) {
		return;
	}

	try {

		LogE("Assert").print("Assertion failed in file '%s' [%d:%d] in function '%s':", location.file_name(), location.line(), location.column(), location.function_name());
		LogE("Assert").print("\t" + message, std::forward<Args>(args)...);

	} catch (...) {

		LogE("Assert") << "Fatal error: Failed to assert";

	}

	arc_abort();

}


#if (ARC_CFG_ASSERT_POLICY == ARC_ASSERT_POLICY_DEBUG && ARC_DEBUG) || (ARC_CFG_ASSERT_POLICY == ARC_ASSERT_POLICY_BUILD && !defined(ARC_CFG_FINAL_BUILD))
	#define ARC_ASSERT_ENABLED
#endif


#ifdef ARC_ASSERT_ENABLED
	#define __ARC_ASSERT_IMPL(cond, msg, ...) __arc_assert(!!(cond), std::source_location::current(), msg __VA_OPT__(,) __VA_ARGS__);
#elif defined(ARC_CFG_ASSERT_ASSUME)
	#define __ARC_ASSERT_IMPL(cond, msg, ...) arc_assume(cond);
#else
	#define __ARC_ASSERT_IMPL(cond, msg, ...)
#endif

#define arc_assert(cond, msg, ...) do { __arc_assert_constexpr(cond); __ARC_ASSERT_IMPL(cond, msg __VA_OPT__(,) __VA_ARGS__) } while (false)

#define arc_force_assert(msg, ...) arc_assert(false, msg __VA_OPT__(,) __VA_ARGS__)
