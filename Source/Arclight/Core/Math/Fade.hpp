/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Fade.hpp
 */

#pragma once

#include "Math.hpp"



namespace Math::Fade {
	
	template<CC::Arithmetic A>
	constexpr A linearIn(A t) noexcept {
		return t;
	}

	template<CC::Arithmetic A>
	constexpr A linearOut(A t) noexcept {
		return t;
	}

	template<CC::Arithmetic A>
	constexpr A linearInOut(A t) noexcept {
		return t;
	}

	
	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quadIn(A t) {
		return Math::pow(t, 2);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quadOut(A t) {
		return -Math::pow(t - 1, 2) + 1;
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quadInOut(A t) {
		return t > 0.5 ? -2 * Math::pow(t - 1, 2) + 1 : 2 * Math::pow(t, 2);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A cubicIn(A t) {
		return Math::pow(t, 3);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A cubicOut(A t) {
		return 1 + Math::pow(t - 1, 3);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A cubicInOut(A t) {
		return t > 0.5 ? 4 * Math::pow(t - 1, 3) + 1 : 4 * Math::pow(t, 3);
	}

	
	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quarticIn(A t) {
		return Math::pow(t, 4);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quarticOut(A t) {
		return 1 - Math::pow(t - 1, 4);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quarticInOut(A t) {
		return t > 0.5 ? -8 * Math::pow(t - 1, 4) + 1 : 8 * Math::pow(t, 4);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quinticIn(A t) {
		return Math::pow(t, 5);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quinticOut(A t) {
		return 1 + Math::pow(t - 1, 5);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A quinticInOut(A t) {
		return t > 0.5 ? 16 * Math::pow(t - 1, 5) + 1 : 16 * Math::pow(t, 5);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A expIn(A t, A b = 5) {

		A eb = Math::exp(b);
		A ebx = Math::pow(eb, t);

		return (ebx - 1) / (eb - 1);

	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A expOut(A t, A b = 5) {

		A eb = Math::exp(-b);
		A ebx = Math::pow(eb, t);

		return (ebx - 1) / (eb - 1);

	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A expInOut(A t, A b = 5) {
		return t > 0.5 ? 0.5 * expOut(2 * t - 1, b) + 0.5 : 0.5 * expIn(2 * t, b);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A circIn(A t) {
		return 1 - Math::sqrt(1 - Math::pow(t, 2));
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A circOut(A t) {
		return Math::sqrt(1 - Math::pow(t - 1, 2));
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A circInOut(A t) {
		return t > 0.5 ? Math::sqrt(0.25 - Math::pow(t - 1, 2)) + 0.5 : 0.5 - Math::sqrt(0.25 - Math::pow(t, 2));
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A backIn(A t, A b = 1.5) {
		return (b + 1) * Math::pow(t, 3) - b * Math::pow(t, 2);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A backOut(A t, A b = 1.5) {
		return (b + 1) * Math::pow(t - 1, 3) + b * Math::pow(t - 1, 2) + 1;
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A backInOut(A t, A b = 1.5) {
		return t > 0.5 ? 4 * (b + 1) * Math::pow(t - 1, 3) + 2 * b * Math::pow(t - 1, 2) + 1 : 4 * (b + 1) * Math::pow(t, 3) - 2 * b * Math::pow(t, 2);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A elasticIn(A t, A b = 5, A c = 3) {
		return (1 - expOut(1 - t, b)) * Math::cos(c * Math::TPi<TT::ToFloat<A>> * (t - 1));
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A elasticOut(A t, A b = 5, A c = 3) {
		return (expOut(t, b) - 1) * Math::cos(c * Math::TPi<TT::ToFloat<A>> * t) + 1;
	}

	// Sigmoidal on the halfway point (to keep continuity of the parameter c)
	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A elasticInOut(A t, A b = 5, A c = 5) {
		return t > 0.5 ? 0.5 * elasticOut(2 * t - 1, b, c) + 0.5 : 0.5 * elasticIn(2 * t, b, c);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A polyIn(A t, A n) {
		return Math::pow(t, n);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A polyOut(A t, A n) {
		return 1 - Math::pow(1 - t, n);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A polyInOut(A t, A n) {
		return t > 0.5 ? 1 - 0.5 * Math::pow(1.5 - t, n) : 0.5 * Math::pow(2 * t, n);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A sineIn(A t) {
		return 1 - Math::cos(t * Math::TPi<TT::ToFloat<A>> / 2);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A sineOut(A t) {
		return Math::sin(t * Math::TPi<TT::ToFloat<A>> / 2);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A sineInOut(A t) {
		return -(Math::cos(t * Math::TPi<TT::ToFloat<A>>) - 1) / 2;
	}


}