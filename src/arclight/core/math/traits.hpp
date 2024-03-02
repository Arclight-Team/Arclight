/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 limits.hpp
 */

#pragma once

#include "fenv.hpp"
#include "common/types.hpp"
#include "common/concepts.hpp"
#include "util/bits.hpp"

#include <limits>
#include <utility>



namespace Math {

	namespace Detail {

		constexpr Rounding RoundingTable[] = {
			Rounding::Unknown,
			Rounding::TowardZero,
			Rounding::Nearest,
			Rounding::Upward,
			Rounding::Downward
		};

	}


	template<class T>
	struct Traits;

	template<class T> requires (CC::Exists<std::numeric_limits<T>>)
	struct Traits<T> {

	private:

		using Limits = std::numeric_limits<T>;

	public:

		using Type = T;

		static constexpr bool Signed			= Limits::is_signed;
		static constexpr bool Integer			= Limits::is_integer;
		static constexpr bool Exact				= Limits::is_exact;
		static constexpr bool HasInfinity		= Limits::has_infinity;
		static constexpr bool HasQuietNaN		= Limits::has_quiet_NaN;
		static constexpr bool HasSignalingNaN	= Limits::has_signaling_NaN;
		static constexpr bool IEC559			= Limits::is_iec559;
		static constexpr bool Bounded			= Limits::is_bounded;
		static constexpr bool Modulo			= Limits::is_modulo;
		static constexpr bool Traps				= Limits::traps;
		static constexpr bool TinynessBefore	= Limits::tinyness_before;

		static constexpr u32 Digits			= Limits::digits;
		static constexpr u32 Digits10		= Limits::digits10;
		static constexpr u32 MaxDigits10	= Limits::max_digits10;
		static constexpr u32 Radix			= Limits::radix;
		static constexpr u32 MinExponent	= Limits::min_exponent;
		static constexpr u32 MinExponent10	= Limits::min_exponent10;
		static constexpr u32 MaxExponent	= Limits::max_exponent;
		static constexpr u32 MaxExponent10	= Limits::max_exponent10;

		static constexpr Rounding RoundStyle = Detail::RoundingTable[Limits::round_style + 1];

		static constexpr T Min			= Limits::min();
		static constexpr T Lowest		= Limits::lowest();
		static constexpr T Max			= Limits::max();
		static constexpr T Epsilon		= Limits::epsilon();
		static constexpr T RoundError	= Limits::round_error();
		static constexpr T Infinity		= Limits::infinity();
		static constexpr T QuietNaN		= Limits::quiet_NaN();
		static constexpr T SignalingNaN	= Limits::signaling_NaN();
		static constexpr T DenormMin	= Limits::denorm_min();

	};

}


namespace CC {

	template<class T>
	concept MathExaminable = Exists<Math::Traits<T>>;

}
