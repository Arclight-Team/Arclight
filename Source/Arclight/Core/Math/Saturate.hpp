/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Saturate.hpp
 */

#pragma once

#include "Overflow.hpp"
#include "Traits.hpp"
#include "Common/Assert.hpp"



namespace Math {

	template<CC::Integral I>
	constexpr I saturateAdd(I a, I b) noexcept {

		auto [value, overflown] = Math::overflowAdd(a, b);

		if (!overflown) {
			return value;
		} else if constexpr (CC::UnsignedIntegral<I>) {
			return Traits<I>::Max;
		} else if (a < 0) {
			return Traits<I>::Min;
		} else {
			return Traits<I>::Max;
		}

	}

	template<CC::Integral I>
	constexpr I saturateSubtract(I a, I b) noexcept {

		auto [value, overflown] = Math::overflowSubtract(a, b);

		if (!overflown) {
			return value;
		} else if constexpr (CC::UnsignedIntegral<I>) {
			return Traits<I>::Min;
		} else if (a < 0) {
			return Traits<I>::Min;
		} else {
			return Traits<I>::Max;
		}

	}

	template<CC::Integral I>
	constexpr I saturateMultiply(I a, I b) noexcept {

		auto [bottom, top, overflown] = Math::overflowMultiply(a, b);

		if (!overflown) {
			return bottom;
		} else if constexpr (CC::UnsignedIntegral<I>) {
			return Traits<I>::Max;
		} else if (a < 0 != b < 0) {
			return Traits<I>::Min;
		} else {
			return Traits<I>::Max;
		}

	}

	template<CC::Integral I>
	constexpr I saturateDivide(I a, I b) noexcept {

		arc_assert(b != 0, "Saturation division by zero");

		if constexpr (CC::UnsignedIntegral<I>) {
			return a / b;
		} else if (a == Traits<I>::Min && b == -1) {
			return Traits<I>::Max;
		} else {
			return a / b;
		}

	}


	template<CC::Integral I, CC::Integral J>
	constexpr I saturateCast(J value) noexcept {

		if constexpr (Traits<J>::Digits > Traits<I>::Digits) {

			if (value > Traits<I>::Max) {
				return Traits<I>::Max;
			}

		}

		if constexpr (CC::SignedIntegral<J>) {

			using S = TT::MakeSigned<TT::CommonType<I, J>>;

			if (S(value) < S(Traits<I>::Min)) {
				return Traits<I>::Min;
			}

		}

		return I(value);

	}

}


#define arc_sat_cast Math::saturateCast
