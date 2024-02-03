/*
 * Copyright (c) 2022-2024 - Arclight Team
 *
 * This file is part of Arclight. All rights reserved.
 *
 * fpintrinsic.hpp
 */

#pragma once

#include "arcintrinsic.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "util/bits.hpp"

#include "common.hpp"
#include "traits.hpp"



namespace CC {

	template<class T>
	concept IEEEFloat = Float<T> && Math::Traits<T>::IEC559;

	template<class T>
	concept IEEEMaskableFloat = IEEEFloat<T> && TT::HasSizedInteger<T>;

	template<class T>
	concept GenericFloat = Float<T> && !IEEEFloat<T>;

	template<class T>
	concept GenericArithmetic = Arithmetic<T> && !IEEEFloat<T>;

}


namespace Math {

	template<CC::IEEEMaskableFloat F>
	struct IEEETraits;

	template<CC::IEEEMaskableFloat F> requires (sizeof(F) == 4) // Binary32
	struct IEEETraits<F> {

		using Type = TT::UnsignedFromSize<4>;

		constexpr static SizeT SignShift = 31;
		constexpr static SizeT ExponentShift = 23;
		constexpr static SizeT MantissaShift = 0;

		constexpr static SizeT ExponentSize = 8;
		constexpr static SizeT MantissaSize = 23;
		constexpr static SizeT ExponentBias = 127;

		constexpr static Type One = 1;

		constexpr static Type SignMask = Bits::createMask<Type>(SignShift, 1);
		constexpr static Type ExponentMask = Bits::createMask<Type>(ExponentShift, ExponentSize);
		constexpr static Type MantissaMask = Bits::createMask<Type>(MantissaShift, MantissaSize);

	};

	template<CC::IEEEMaskableFloat F> requires (sizeof(F) == 8) // Binary64
	struct IEEETraits<F> {

		using Type = TT::UnsignedFromSize<8>;

		constexpr static SizeT SignShift = 63;
		constexpr static SizeT ExponentShift = 52;
		constexpr static SizeT MantissaShift = 0;

		constexpr static SizeT ExponentSize = 11;
		constexpr static SizeT MantissaSize = 52;
		constexpr static SizeT ExponentBias = 1023;

		constexpr static Type One = 1;

		constexpr static Type SignMask = Bits::createMask<Type>(SignShift, 1);
		constexpr static Type ExponentMask = Bits::createMask<Type>(ExponentShift, ExponentSize);
		constexpr static Type MantissaMask = Bits::createMask<Type>(MantissaShift, MantissaSize);

	};

	template<CC::IEEEMaskableFloat F> requires (sizeof(F) == 10) // Extended80
	struct IEEETraits<F> {

		using Type = u8;

		constexpr static SizeT SignShift = 7;       // 79
		constexpr static SizeT ExponentShift = 0;   // 64
		constexpr static SizeT MantissaShift = 0;   // 0

		constexpr static SizeT ExponentSize = 15;
		constexpr static SizeT MantissaSize = 64;
		constexpr static SizeT ExponentBias = 16383;

		constexpr static Type One = 1;

	};

	template<CC::IEEEMaskableFloat F> requires (sizeof(F) == 16) // Binary128
	struct IEEETraits<F> {

		using Type = u8;

		constexpr static SizeT SignShift = 7;       // 127
		constexpr static SizeT ExponentShift = 0;   // 112
		constexpr static SizeT MantissaShift = 0;   // 0

		constexpr static SizeT ExponentSize = 15;
		constexpr static SizeT MantissaSize = 112;
		constexpr static SizeT ExponentBias = 16383;

		constexpr static Type One = 1;

	};

}


namespace Math::Intrinsic {

	template<CC::IEEEFloat F> static constexpr F PInf = Traits<F>::Infinity;
	template<CC::IEEEFloat F> static constexpr F NInf = -Traits<F>::Infinity;
	template<CC::IEEEFloat F> static constexpr F QNaN = Traits<F>::QuietNaN;
	template<CC::IEEEFloat F> static constexpr F SNaN = Traits<F>::SignalingNaN;
	template<CC::IEEEFloat F> static constexpr F PZero = 0;
	template<CC::IEEEFloat F> static constexpr F NZero = Bits::cast<F>(IEEETraits<F>::SignMask);


	#pragma region Utility

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr auto toRaw(F f) noexcept {
		return Bits::cast<typename E::Type>(f);
	}


	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F makeSignaling(F f) noexcept {
		return Bits::cast<F>(toRaw(f) & ~(E::One << (E::MantissaSize - 1)));
	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F makeQuiet(F f) noexcept {
		return Bits::cast<F>(toRaw(f) | (E::One << (E::MantissaSize - 1)));
	}

	#pragma endregion


	#pragma region Classification and comparison

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr bool isSpecialValueOrZero(F f) noexcept {
		return ((toRaw(f) & ~E::SignMask) - 1) >= (E::ExponentMask - 1);
	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr bool isSpecialValue(F f) noexcept {
		return (toRaw(f) & ~E::SignMask) >= E::ExponentMask;
	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr bool isNormal(F f) noexcept {
		return ((toRaw(f) - (E::One << E::ExponentShift)) & ~E::SignMask) < (E::ExponentMask - (E::One << E::ExponentShift));
	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr bool isZero(F f) noexcept {
		return (toRaw(f) & ~E::SignMask) == 0;
	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr bool isInfinity(F f) noexcept {
		return (toRaw(f) & ~E::SignMask) == E::ExponentMask;
	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr bool isNaN(F f) noexcept {
		return (toRaw(f) & ~E::SignMask) > E::ExponentMask;
	}


	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr Category classify(F x) noexcept {

		using T = typename E::Type;

		if (x == 0) {
			return Category::Zero;
		}

		T ix = toRaw(x);
		T ex = (ix & E::ExponentMask) >> E::ExponentShift;

		if (!isSpecialValue(x)) {

			return (ix & E::ExponentMask) >> E::ExponentShift ? Category::Normal : Category::Subnormal;

		} else {

			if (isNaN(x)) {
				return Category::NaN;
			} else {
				return Category::Infinity;
			}

		}

	}


	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F max(F x, F y) noexcept {

		using T = typename E::Type;

		if (isNaN(x)) {
			return y;
		} else if (isNaN(y)) {
			return x;
		}

		T ix = toRaw(x);
		T iy = toRaw(y);

		if (!((ix | iy) & ~E::SignMask)) {
			return ix <= iy ? x : y;
		}

		return x >= y ? x : y;

	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F min(F x, F y) noexcept {

		using T = typename E::Type;

		if (isNaN(x)) {
			return y;
		} else if (isNaN(y)) {
			return x;
		}

		T ix = toRaw(x);
		T iy = toRaw(y);

		if (!((ix | iy) & ~E::SignMask)) {
			return ix >= iy ? x : y;
		}

		return x <= y ? x : y;

	}


	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr bool signbit(F f) noexcept {
		return toRaw(f) & E::SignMask;
	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F copySign(F from, F to) noexcept {
		return Bits::cast<F>((toRaw(from) & E::SignMask) | (toRaw(to) & ~E::SignMask));
	}

	#pragma endregion


	#pragma region Basic operations

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F abs(F x) noexcept {

		using T = typename E::Type;

		T y = Bits::cast<T>(x);
		y &= ~E::SignMask;

		return Bits::cast<F>(y);

	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F mod(F x, F y) noexcept {

		using T = typename E::Type;
		using I = TT::MakeSigned<T>;

		constexpr SizeT DiscardingZeroes = Bits::bitCount<F>() - E::ExponentShift;


		// Obtain integer representations
		T ix = toRaw(x);
		T iy = toRaw(y);

		// Absolute values
		T ax = ix & ~E::SignMask;
		T ay = iy & ~E::SignMask;

		// Sign of x
		T sx = ix & E::SignMask;


		// First, deal with special values
		if (isSpecialValueOrZero(x) || isSpecialValueOrZero(y)) {

			// If any operand is nan, the result will be nan as well
			if (isNaN(x) || isNaN(y)) {
				return x + y;   //Propagate the existing nan
			}

			// If x is inf or y is 0, return nan
			if (isInfinity(x) || y == 0) {
				return Bits::cast<F>(Bits::cast<T>(QNaN<F>) | sx);
			}

			// If x is 0 or y is inf, return x
			// This case is trapped in this branch so no extra check is necessary
			return x;

		}

		// To start the division process, exclude the case where y >= x
		if (ay >= ax) {

			// Case y > x: Return x
			// Case y == x: Return 0 with the sign of x
			if (ay > ax) {
				return x;
			} else {
				return Bits::cast<F>(sx);
			}

		}

		// The last case to check for is subnormals
		// First, get the exponent
		I ex = ax >> E::ExponentShift;
		I ey = ay >> E::ExponentShift;

		// Then get the mantissa
		T mx = ax & E::MantissaMask;
		T my = ay & E::MantissaMask;

		// If the exponent is zero it must be a subnormal (zero is covered above)
		if (!ex) {

			// Shift to normalize the number, then adjust the exponent
			I z = Bits::clz(ax) - DiscardingZeroes;
			mx <<= z + 1;
			ex -= z;

		} else {

			// Set implicit mantissa bit
			mx |= E::One << E::ExponentShift;

		}

		// Apply the same logic to y
		if (!ey) {

			I z = Bits::clz(ay) - DiscardingZeroes;
			my <<= z + 1;
			ey -= z;

		} else {

			my |= E::One << E::ExponentShift;

		}


		// The exponent difference: We can shift the dividend's mantissa by this amount
		I expDelta = ex - ey;

		// If the divisor's mantissa is greater than the dividend's, we have to shift once more
		I reshift = my > mx ? 1 : 0;

		// As long as we didn't exhaust our exponent delta
		while (reshift <= expDelta) {

			expDelta -= reshift;    // Reduce delta
			mx <<= reshift;         // Shift so that mx > my
			mx -= my;               // Subtract (binary division step)

			// If mx is prematurely 0, the result is +0/-0
			if (mx == 0) {
				return Bits::cast<F>(sx);
			}

			reshift = Bits::clz(mx) - DiscardingZeroes + 1;     // Calculate the new shift
			reshift += (mx << reshift) < my;                    // Add one in case our new mx is still smaller than my

		}


		// Apply the remaining delta if it isn't exhausted yet
		mx <<= expDelta;

		// If our divisor's exponent has been 0 or less it must have been subnormal
		if (ey <= 0) {

			// Shift right to compensate the (potentially) negative exponent
			mx >>= -ey + 1;
			return Bits::cast<F>(sx | (mx & E::MantissaMask));

		} else {

			// The number is normal, calculate the renormalization shift
			// Note that mx is always less than a normal number since we left the loop with a subtraction (so mx will always fit the mantissa)
			I renormalizeShift = Bits::clz(mx) - DiscardingZeroes + 1;
			I er = 0;

			// We may not shift more than ey allows us
			if (renormalizeShift >= ey) {

				// Subnormal shift, one less ey since we must keep the trailing 1 in the mantissa
				// The subnormal transformation happens through er = 0
				mx <<= ey - 1;

			} else {

				// New exponent is ey adjusted by our renormalization shift
				er = ey - renormalizeShift;

				// Shift the mantissa
				mx <<= renormalizeShift;

			}

			// Reassemble the result
			T r = sx | (er << E::ExponentShift) | (mx & E::MantissaMask);
			return Bits::cast<F>(r);

		}

	}

	#pragma endregion


	#pragma region Rounding functions

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F trunc(F x) noexcept {

		if (!std::is_constant_evaluated()) {

#ifdef ARC_TARGET_HAS_SSE4_1

			if constexpr (CC::Equal<F, float>) {

				__m128 v = _mm_set_ss(x);
				return _mm_cvtss_f32(_mm_round_ss(v, v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC));

			} else if constexpr (CC::Equal<F, double>) {

				__m128d v = _mm_set_sd(x);
				return _mm_cvtsd_f64(_mm_round_sd(v, v, _MM_FROUND_TO_ZERO | _MM_FROUND_NO_EXC));

			}
#endif

		}

		using T = typename E::Type;

		T ix = toRaw(x);
		T ex = ix & E::ExponentMask;

		constexpr T T0 = E::ExponentBias << E::ExponentShift;
		constexpr T T1 = (E::ExponentBias + E::MantissaSize) << E::ExponentShift;

		if (ex < T0) {

			// Below one, discard to 0
			return copySign(x, F(0));

		} else if (ex < T1) {

			// Clear all fractional mantissa bits
			T s = (T1 - ex) >> E::ExponentShift;
			return Bits::cast<F>(Bits::mask(ix, s));

		} else if (isNaN(x)) {

			return makeQuiet(x);

		}

		// x is already an integer
		return x;

	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F ceil(F x) noexcept {

		if (!std::is_constant_evaluated()) {

#ifdef ARC_TARGET_HAS_SSE4_1

			if constexpr (CC::Equal<F, float>) {

				__m128 v = _mm_set_ss(x);
				return _mm_cvtss_f32(_mm_round_ss(v, v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC));

			} else if constexpr (CC::Equal<F, double>) {

				__m128d v = _mm_set_sd(x);
				return _mm_cvtsd_f64(_mm_round_sd(v, v, _MM_FROUND_TO_POS_INF | _MM_FROUND_NO_EXC));

			}

#endif

		}

		using T = typename E::Type;

		T ix = toRaw(x);
		T ex = ix & E::ExponentMask;

		constexpr T T0 = E::ExponentBias << E::ExponentShift;
		constexpr T T1 = (E::ExponentBias + E::MantissaSize) << E::ExponentShift;

		if (ex < T0) {

			// Below one, return -0 for negatives, 0 for 0 and 1 for positives
			return ix & E::SignMask ? NZero<F> : (ix == 0 ? 0 : 1);

		} else if (ex < T1) {

			// Clear all fractional mantissa bits
			T s = (T1 - ex) >> E::ExponentShift;
			F f = Bits::cast<F>(Bits::mask(ix, s));

			// Add one if x wasn't an integer already since positive values need to be rounded up
			if (x >= 0 && x != f) {
				f += 1;
			}

			return f;

		} else if (isNaN(x)) {

			return makeQuiet(x);

		}

		// x is already an integer
		return x;

	}

	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F floor(F x) noexcept {

		if (!std::is_constant_evaluated()) {

#ifdef ARC_TARGET_HAS_SSE4_1

			if constexpr (CC::Equal<F, float>) {

				__m128 v = _mm_set_ss(x);
				return _mm_cvtss_f32(_mm_round_ss(v, v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));

			} else if constexpr (CC::Equal<F, double>) {

				__m128d v = _mm_set_sd(x);
				return _mm_cvtsd_f64(_mm_round_sd(v, v, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC));

			}

#endif

		}

		using T = typename E::Type;

		T ix = toRaw(x);
		T ex = ix & E::ExponentMask;

		constexpr T T0 = E::ExponentBias << E::ExponentShift;
		constexpr T T1 = (E::ExponentBias + E::MantissaSize) << E::ExponentShift;

		if (ex < T0) {

			// Below one, return -1 for negatives, -0 for -0 and 0 for positives
			return ix & E::SignMask ? (x == 0 ? NZero<F> : -1) : 0;

		} else if (ex < T1) {

			// Clear all fractional mantissa bits
			T s = (T1 - ex) >> E::ExponentShift;
			F f = Bits::cast<F>(Bits::mask(ix, s));

			// Subtract one if x wasn't an integer already since negative values need to be rounded down
			if (x < 0 && x != f) {
				f -= 1;
			}

			return f;

		} else if (isNaN(x)) {

			return makeQuiet(x);

		}

		// x is already an integer
		return x;

	}

	// Rounds torward signed infinity (x >= 0 ? ceil : floor)
	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F extrude(F x) noexcept {

		if (!std::is_constant_evaluated()) {

			// If we have fast ceil/floor, take these
#ifdef ARC_TARGET_HAS_SSE4_1

			if constexpr (CC::Equal<F, float> || CC::Equal<F, double>) {
				return x >= 0 ? ceil(x) : floor(x);
			}
			
#endif

		}

		using T = typename E::Type;

		T ix = toRaw(x);
		T ex = ix & E::ExponentMask;

		constexpr T T0 = E::ExponentBias << E::ExponentShift;
		constexpr T T1 = (E::ExponentBias + E::MantissaSize) << E::ExponentShift;

		if (ex < T0) {

			// Below one, return +-O for zero, else +-1
			return ix & ~E::SignMask ? copySign(x, F(1)) : x;

		} else if (ex < T1) {

			// Clear all fractional mantissa bits
			T s = (T1 - ex) >> E::ExponentShift;
			F f = Bits::cast<F>(Bits::mask(ix, s));

			// Add/Subtract one if x wasn't an integer already
			if (x != f) {
				f += copySign(x, F(1));
			}

			return f;

		} else if (isNaN(x)) {

			return makeQuiet(x);

		}

		// x is already an integer
		return x;

	}

	// Rounds to the nearest integer where halfway cases are rounded away from zero, i.e. 0.5 becomes 1.0, -0.5 becomes -1.0
	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F round(F x) noexcept {

		using T = typename E::Type;

		T ix = toRaw(x);
		T ex = ix & E::ExponentMask;

		constexpr T T0 = (E::ExponentBias - 1) << E::ExponentShift;
		constexpr T T1 = (E::ExponentBias + E::MantissaSize) << E::ExponentShift;

		if (ex < T0) {

			// Below 0.5, return -0 for negatives and 0 for positives
			return Bits::cast<F>(ix & E::SignMask);

		} else if (ex < T1) {

			// Add/Subtract 0.5
			F y = x + copySign(x, F(0.5));
			T iy = toRaw(y);
			T ey = iy & E::ExponentMask;

			// Truncate
			T s = (T1 - ey) >> E::ExponentShift;
			return Bits::cast<F>(Bits::mask(iy, s));

		}

		// x is already an integer
		return x;

	}

	#pragma endregion


	#pragma region Miscellaneous

	// Next representable float value
	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F nextFloat(F x) noexcept {

		using T = typename E::Type;

		T ix = toRaw(x);

		if (isNaN(x)) {
			return x;
		}

		if (ix & E::SignMask) {
			return ix == E::SignMask ? 0 : Bits::cast<F>(ix - 1);
		} else {
			return Bits::cast<F>(ix + 1);
		}

	}

	// Previous representable float value
	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr F prevFloat(F x) noexcept {

		using T = typename E::Type;

		T ix = toRaw(x);

		if (isNaN(x)) {
			return x;
		}

		if (ix & E::SignMask) {
			return Bits::cast<F>(ix + 1);
		} else {
			return ix == 0 ? NZero<F> : Bits::cast<F>(ix - 1);
		}

	}


	// Distance between two floats in ULPs (NaN results in -1)
	template<CC::IEEEMaskableFloat F, class E = IEEETraits<F>>
	constexpr typename E::Type ulpDistance(F x, F y) noexcept {

		using T = typename E::Type;

		T ix = toRaw(x);
		T iy = toRaw(y);

		if (isNaN(x) || isNaN(y)) {
			return -1;
		}

		if ((ix ^ iy) & E::SignMask) {

			// We have different signs
			return ulpDistance(x, copySign(x, PZero<F>)) + ulpDistance(y, copySign(y, PZero<F>)) + 1;

		}

		if (ix >= iy) {
			return ix - iy;
		} else {
			return iy - ix;
		}

	}

	#pragma endregion

}