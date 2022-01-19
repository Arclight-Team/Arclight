/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 traits.hpp
 */

#pragma once

#include "util/concepts.hpp"


namespace Math {

	consteval bool Even(Integral auto x) noexcept {
		return !Odd(x);
	}

	consteval bool Odd(Integral auto x) noexcept {
		return x & 1;
	}

	consteval bool Divisible(Integral auto dividend, Integral auto divisor) noexcept {
		return dividend / divisor * divisor == dividend;
	}

}