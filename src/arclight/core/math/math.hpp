/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 math.hpp
 */

#pragma once

#include "types.hpp"
#include "arcconfig.hpp"
#include "arcintrinsic.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "constants.hpp"
#include "traits.hpp"
#include "fpintrinsic.hpp"

#include <cmath>
#include <numeric>
#include <limits>
#include <array>
#include <algorithm>



namespace Math {

	template<CC::Float F>
	struct SplitFloat {

		F integer;
		F fractional;

	};

	template<CC::Integral I>
	struct DivisionResult {

		I quotient;
		I remainder;

	};

	enum class ConstantType {
		Zero,
		NaN,
		Infinity
	};


	#pragma region Algorithm

	template<CC::Arithmetic A, CC::Arithmetic B, CC::GenericArithmetic C = TT::CommonType<A, B>>
	constexpr C min(A a, B b) {
		return std::min<C>(a, b);
	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::IEEEMaskableFloat C = TT::CommonType<A, B>>
	constexpr C min(A a, B b) {
		return Intrinsic::min<C>(a, b);
	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic... Args>
	constexpr TT::CommonType<A, B, Args...> min(A a, B b, Args... args) {
		return min(min(a, b), args...);
	}


	template<CC::Arithmetic A, CC::Arithmetic B, CC::GenericArithmetic C = TT::CommonType<A, B>>
	constexpr C max(A a, B b) {
		return std::max<C>(a, b);
	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::IEEEMaskableFloat C = TT::CommonType<A, B>>
	constexpr C max(A a, B b) {
		return Intrinsic::max<C>(a, b);
	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic... Args>
	constexpr TT::CommonType<A, B, Args...> max(A a, B b, Args... args) {
		return max(max(a, b), args...);
	}


	template<CC::Arithmetic A, CC::Arithmetic... Args> requires (sizeof...(Args) > 0)
	constexpr std::array<TT::CommonType<A, Args...>, 2> minmax(A a, Args... args) {
		return {min(a, args...), max(a, args...)};
	}

	#pragma endregion


	#pragma region Basic operations

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 F abs(F value) noexcept {
		return std::fabs(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr F abs(F value) noexcept {
		return Intrinsic::abs(value);
	}

	template<CC::Integral I>
	constexpr I abs(I value) noexcept {

		if constexpr (CC::UnsignedIntegral<I>) {

			return value;

		} else if constexpr (CC::SignedIntegral<I>) {

			TT::MakeUnsigned<I> y = value >> (Bits::bitCount<I>() - 1);
			return (value ^ y) + (y & 1);

		}

	}


	template<CC::Arithmetic F, CC::Arithmetic G, CC::GenericFloat C = TT::CommonType<F, G>>
	ARC_CONSTEXPR_CMATH23 C mod(F a, G b) noexcept {
		return std::fmod(a, b);
	}

	template<CC::Arithmetic F, CC::Arithmetic G, CC::IEEEMaskableFloat C = TT::CommonType<F, G>>
	constexpr C mod(F a, G b) noexcept {
		return Intrinsic::mod<C>(a, b);
	}

	template<CC::Integral I, CC::Integral J>
	constexpr TT::CommonType<I, J> mod(I a, J b) noexcept {
		return a % b;
	}


	template<CC::Integral I, CC::Integral J>
	constexpr DivisionResult<TT::CommonType<I, J>> divmod(I a, J b) noexcept {
		return {a / b, mod(a, b)};
	}


	template<CC::Arithmetic A, CC::Arithmetic B>
	ARC_CONSTEXPR_CMATH23 TT::CommonType<A, B> rem(A a, B b) noexcept {
		return std::remainder(a, b);
	}

	#pragma endregion


	#pragma region Trigonometric functions

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A sin(A radians) noexcept {
		return std::sin(radians);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A cos(A radians) noexcept {
		return std::cos(radians);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A tan(A radians) noexcept {
		return std::tan(radians);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A csc(A radians) noexcept {

		A res = sin(radians);

		if (isZero(res)) {
			return copySign(Traits<A>::Infinity, radians);
		}

		return A(1) / res;

	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A sec(A radians) noexcept {

		A res = cos(radians);

		if (isZero(res)) {
			return copySign(Traits<A>::Infinity, radians);
		}

		return A(1) / res;

	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A cot(A radians) noexcept {

		A res = tan(radians);

		if (isZero(res)) {
			return copySign(Traits<A>::Infinity, radians);
		}

		return A(1) / res;

	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A asin(A value) noexcept {
		return std::asin(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A acos(A value) noexcept {
		return std::acos(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A atan(A value) noexcept {
		return std::atan(value);
	}

	template<CC::Arithmetic A, CC::Arithmetic B>
	ARC_CONSTEXPR_CMATH26 TT::CommonType<A, B> atan2(A y, B x) noexcept {
		return std::atan2(y, x);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A sinh(A value) noexcept {
		return std::sinh(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A cosh(A value) noexcept {
		return std::cosh(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A tanh(A value) noexcept {
		return std::tanh(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A asinh(A value) noexcept {
		return std::asinh(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A acosh(A value) noexcept {
		return std::acosh(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A atanh(A value) noexcept {
		return std::atanh(value);
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 std::array<A, 2> sincos(A radians) noexcept {
		return {sin(radians), cos(radians)};
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 std::array<A, 2> asincos(A radians) noexcept {
		return {asin(radians), acos(radians)};
	}


	template<CC::Arithmetic A>
	constexpr TT::ToFloat<A> toDegrees(A radians) noexcept {
		return radians * 180 / TPi<TT::ToFloat<A>>;
	}

	template<CC::Arithmetic A>
	constexpr TT::ToFloat<A> toRadians(A degrees) noexcept {
		return degrees * TPi<TT::ToFloat<A>> / 180;
	}

	#pragma endregion


	#pragma region Exponential functions

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A exp(A exponent) noexcept {
		return std::exp(exponent);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A ln(A value) noexcept {
		return std::log(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A log(A value) noexcept {
		return std::log10(value);
	}


	template<CC::Arithmetic A, CC::Arithmetic B>
	ARC_CONSTEXPR_CMATH26 TT::CommonType<A, B> log(A base, B value) noexcept {
		return log(value) / log(base);
	}

	#pragma endregion


	#pragma region Power functions

	template<CC::Arithmetic A, CC::Arithmetic B>
	ARC_CONSTEXPR_CMATH26 TT::CommonType<A, B> pow(A base, B exponent) noexcept {
		return std::pow(base, exponent);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A sqrt(A value) noexcept {
		return std::sqrt(value);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A cbrt(A value) noexcept {
		return std::cbrt(value);
	}

	#pragma endregion


	#pragma region Rounding functions

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 F ceil(F value) noexcept {
		return std::ceil(value);
	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 F floor(F value) noexcept {
		return std::floor(value);
	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 F trunc(F value) noexcept {
		return std::trunc(value);
	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 F round(F value) noexcept {
		return std::round(value);
	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 F extrude(F value) noexcept {
		return value >= 0 ? ceil(value) : floor(value);
	}


	template<CC::IEEEMaskableFloat F>
	constexpr F ceil(F value) noexcept {
		return Intrinsic::ceil(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr F floor(F value) noexcept {
		return Intrinsic::floor(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr F trunc(F value) noexcept {
		return Intrinsic::trunc(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr F round(F value) noexcept {
		return Intrinsic::round(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr F extrude(F value) noexcept {
		return Intrinsic::extrude(value);
	}


	template<CC::Integral I>
	constexpr I ceil(I value) noexcept {
		return value;
	}

	template<CC::Integral I>
	constexpr I floor(I value) noexcept {
		return value;
	}

	template<CC::Integral I>
	constexpr I trunc(I value) noexcept {
		return value;
	}

	template<CC::Integral I>
	constexpr I round(I value) noexcept {
		return value;
	}

	template<CC::Integral I>
	constexpr I extrude(I value) noexcept {
		return value;
	}


	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A round(A value, u32 digits) noexcept {
		return (value * pow(10, digits) + 0.5) / pow(10, digits);
	}

	#pragma endregion


	#pragma region Comparison

	namespace Detail {

		static constexpr double Epsilon = 0.000001;
		static constexpr double MinEpsilon = 0.00000001;


		template<bool G, CC::Arithmetic A, CC::Arithmetic B> requires (CC::Float<A> || CC::Float<B>)
		constexpr bool compare(A a, B b) noexcept {

			A y = abs(a);
			B z = abs(b);

			return (G ? a - b : b - a) > (max(y, z) * Epsilon);

		}

		template<bool G, CC::Arithmetic A, CC::Arithmetic B> requires (CC::Float<A> || CC::Float<B>)
		constexpr bool compareEqual(A a, B b) noexcept {

			A y = abs(a);
			B z = abs(b);

			return (G ? a - b : b - a) > -max(max(y, z) * Epsilon, MinEpsilon);

		}

	}


	template<CC::Arithmetic A, CC::Arithmetic B> requires (CC::Float<A> || CC::Float<B>)
	constexpr bool equal(A a, B b) noexcept {

		auto x = abs(a - b);
		A y = abs(a);
		B z = abs(b);

		return x <= (max(max(y, z) * Detail::Epsilon, Detail::MinEpsilon));

	}

	template<CC::Arithmetic A, CC::Arithmetic B> requires (CC::Float<A> || CC::Float<B>)
	constexpr bool less(A a, B b) noexcept {
		return Detail::compare<false>(a, b);
	}

	template<CC::Arithmetic A, CC::Arithmetic B> requires (CC::Float<A> || CC::Float<B>)
	constexpr bool greater(A a, B b) noexcept {
		return Detail::compare<true>(a, b);
	}

	template<CC::Arithmetic A, CC::Arithmetic B> requires (CC::Float<A> || CC::Float<B>)
	constexpr bool lessEqual(A a, B b) noexcept {
		return Detail::compareEqual<false>(a, b);
	}

	template<CC::Arithmetic A, CC::Arithmetic B> requires (CC::Float<A> || CC::Float<B>)
	constexpr bool greaterEqual(A a, B b) noexcept {
		return Detail::compareEqual<true>(a, b);
	}


	template<CC::Integral I, CC::Integral J>
	constexpr bool equal(I a, J b) noexcept {
		return a == b;
	}

	template<CC::Integral I, CC::Integral J>
	constexpr bool less(I a, J b) noexcept {
		return a < b;
	}

	template<CC::Integral I, CC::Integral J>
	constexpr bool greater(I a, J b) noexcept {
		return a > b;
	}

	template<CC::Integral I, CC::Integral J>
	constexpr bool lessEqual(I a, J b) noexcept {
		return a <= b;
	}

	template<CC::Integral I, CC::Integral J>
	constexpr bool greaterEqual(I a, J b) noexcept {
		return a >= b;
	}


	constexpr i32 signFactor(bool negative) noexcept {
		return (1 - negative * 2);
	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 bool signbit(F value) noexcept {
		return std::signbit(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr bool signbit(F value) noexcept {
		return Intrinsic::signbit(value);
	}

	template<CC::Integral I>
	constexpr bool signbit(I value) noexcept {
		return value < I(0);
	}

	template<CC::Arithmetic A>
	constexpr i32 sign(A value) noexcept {
		return 1 - signbit(value) * 2;
	}


	template<CC::Arithmetic A, CC::Arithmetic B, CC::GenericFloat C = TT::CommonType<A, B>>
	ARC_CONSTEXPR_CMATH23 C copySign(A value, B sign) noexcept {
		return std::copysign(value, sign);
	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::IEEEMaskableFloat C = TT::CommonType<A, B>>
	constexpr C copySign(A value, B sign) noexcept {
		return Intrinsic::copySign<C>(sign, value);
	}

	template<CC::Integral I>
	constexpr I copySign(I value, I sign) noexcept {
		return Math::sign(value) == Math::sign(sign) ? value : -value;
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH23 A ldexp(A value, i32 exp) noexcept {
		return std::ldexp(value, exp);
	}


	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 Category classify(F value) noexcept {

		switch (std::fpclassify(value)) {
			case FP_ZERO:		return Category::Zero;
			case FP_SUBNORMAL:	return Category::Subnormal;
			case FP_NORMAL:		return Category::Normal;
			case FP_INFINITE:	return Category::Infinity;
			case FP_NAN:		return Category::NaN;
			default:			std::unreachable();
		}

	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 bool isFinite(F value) noexcept {
		return std::isfinite(value);
	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 bool isNormal(F value) noexcept {
		return std::isnormal(value);
	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 bool isNaN(F value) noexcept {
		return std::isnan(value);
	}

	template<CC::GenericFloat F>
	constexpr bool isInfinity(F value) noexcept {

		if constexpr (std::is_constant_evaluated() && Traits<F>::HasInfinity) {
			return abs(value) == Traits<F>::Infinity;
		}

		return std::isinf(value);

	}

	template<CC::GenericFloat F>
	constexpr bool isInfinity(F value, bool negative) noexcept {

		i32 s = signFactor(negative);

		if constexpr (std::is_constant_evaluated() && Traits<F>::HasInfinity) {
			return value == (Traits<F>::Infinity * s);
		}

		return std::isinf(value) && sign(value) == s;

	}


	template<CC::IEEEMaskableFloat F>
	constexpr Category classify(F value) noexcept {
		return Intrinsic::classify(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr bool isFinite(F value) noexcept {
		return !Intrinsic::isSpecialValue(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr bool isNormal(F value) noexcept {
		return Intrinsic::isNormal(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr bool isNaN(F value) noexcept {
		return Intrinsic::isNaN(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr bool isInfinity(F value) noexcept {
		return Intrinsic::isInfinity(value);
	}

	template<CC::IEEEMaskableFloat F>
	constexpr bool isInfinity(F value, bool negative) noexcept {
		return Intrinsic::isInfinity(value) && (signFactor(negative) == sign(value));
	}


	template<CC::Float F>
	constexpr bool isPositiveInfinity(F value) noexcept {
		return isInfinity(value, false);
	}

	template<CC::Float F>
	constexpr bool isNegativeInfinity(F value) noexcept {
		return isInfinity(value, true);
	}


	template<CC::Float F>
	constexpr bool isZero(F value) noexcept {
		return abs(value) < Detail::MinEpsilon;
	}

	template<CC::Integral I>
	constexpr bool isZero(I value) noexcept {
		return value == I(0);
	}

	#pragma endregion


	#pragma region Miscellaneous

	namespace Exact {

		template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
		constexpr TT::CommonType<A, B, C> lerp(A start, B end, C factor) noexcept {
			return std::lerp(start, end, factor);
		}

		template<CC::Arithmetic A, CC::Arithmetic B>
		constexpr TT::CommonType<A, B> midpoint(A a, B b) noexcept {
			return std::midpoint(a, b);
		}

	}


	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	constexpr TT::CommonType<A, B, C> lerp(A start, B end, C factor) noexcept {
		return start + factor * (end - start);
	}

	template<CC::Arithmetic A, CC::Arithmetic B>
	constexpr TT::CommonType<A, B> midpoint(A a, B b) noexcept {
		return (a + b) / 2;
	}


	template<CC::Arithmetic A, CC::Arithmetic B>
	constexpr TT::CommonType<A, B> gcd(A a, B b) noexcept {
		return std::gcd(a, b);
	}

	template<CC::Arithmetic A, CC::Arithmetic B>
	constexpr TT::CommonType<A, B> lcm(A a, B b) noexcept {
		return std::lcm(a, b);
	}


	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 SplitFloat<F> split(F value) {

		SplitFloat<F> s;
		s.fractional = std::modf(value, &s.integer);

		return s;

	}

	template<CC::GenericFloat F>
	ARC_CONSTEXPR_CMATH23 F fract(F value) {
		return split(value).fractional;
	}


	template<CC::IEEEMaskableFloat F>
	constexpr SplitFloat<F> split(F value) noexcept {

		SplitFloat<F> s;
		s.integer = trunc(value);
		s.fractional = copySign(isInfinity(value) ? F(0) : value - s.integer, value);

		return s;

	}

	template<CC::IEEEMaskableFloat F>
	constexpr F fract(F value) noexcept {
		return split(value).fractional;
	}


	template<CC::Arithmetic A>
	constexpr void ascend(A& a, A& b) noexcept {

		if (less(b, a)) {
			std::swap(a, b);
		}

	}

	template<CC::Arithmetic A>
	constexpr void descend(A& a, A& b) noexcept {

		if (less(a, b)) {
			std::swap(a, b);
		}

	}


	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C, CC::Arithmetic D, CC::Arithmetic E>
	constexpr TT::CommonType<A, B, C, D, E> map(A value, B start1, C end1, D start2, E end2) noexcept {

		using F = TT::ToFloat<TT::CommonType<A, B, C, D, E>>;

		F e = F(end2) - start2;
		F s = F(end1) - start1;
		F n = F(value) - start1;

		return start2 + e * (n / s);

	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	constexpr TT::CommonType<A, B, C> clamp(A value, B min, C max) noexcept {
		return less(value, min) ? min : (greater(value, max) ? max : value);
	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	constexpr bool inRange(A value, B lower, C upper) noexcept {
		return greaterEqual(value, lower) && lessEqual(value, upper);
	}


	template<CC::Arithmetic A>
	constexpr A triangle(A t) noexcept {
		return abs(mod(t - 1, 2.0f) - 1);
	}

	#pragma endregion


	template<ConstantType Y, bool N = false>
	class ArithmeticConstant {

	public:

		static constexpr ConstantType Type = Y;
		static constexpr bool Negative = N;


		constexpr ArithmeticConstant() noexcept = default;


		template<CC::Arithmetic A> requires(Type == ConstantType::Zero)
		constexpr auto operator<=>(A other) const {

			if (*this == other) {
				return std::strong_ordering::equal;
			}

			return greater(0, other) ? std::strong_ordering::greater : std::strong_ordering::less;

		}

		template<CC::Arithmetic A> requires(Type == ConstantType::Zero)
		constexpr bool operator==(A other) const {
			return isZero(other);
		}


		template<CC::Arithmetic A> requires(Type == ConstantType::NaN)
		constexpr auto operator<=>(A other) const noexcept {
			return std::partial_ordering::unordered;
		}

		template<CC::Arithmetic A> requires(Type == ConstantType::NaN)
		constexpr bool operator==(A other) const noexcept {
			return false;
		}


		template<CC::Arithmetic A> requires(Type == ConstantType::Infinity)
		constexpr auto operator<=>(A other) const {

			if (*this == other) {
				return std::strong_ordering::equal;
			}

			return (sign(other) == signFactor(Negative)) ? std::strong_ordering::greater : std::strong_ordering::less;

		}

		template<CC::Arithmetic A> requires(Type == ConstantType::Infinity)
		constexpr bool operator==(A other) const {
			return isInfinity(other, Negative);
		}


		constexpr ArithmeticConstant<Type, !Negative> operator-() const noexcept {
			return {};
		}


		template<ConstantType C, bool E>
		consteval bool operator==(ArithmeticConstant<C, E> constant) const noexcept {

			if constexpr (Type == ConstantType::NaN || C == ConstantType::NaN) {
				return false;
			}

			return (Type == C) && (Type == ConstantType::Zero || Negative == E);

		}

	};

	static constexpr ArithmeticConstant<ConstantType::Zero> Zero;
	static constexpr ArithmeticConstant<ConstantType::NaN> NaN;
	static constexpr ArithmeticConstant<ConstantType::Infinity> Infinity;

}


namespace Math::inline Literals {

	constexpr float operator""_degf(long double degrees) noexcept {
		return Math::toRadians(float(degrees));
	}

	constexpr float operator""_degf(unsigned long long degrees) noexcept {
		return Math::toRadians(float(degrees));
	}

	constexpr float operator""_radf(long double radians) noexcept {
		return Math::toDegrees(float(radians));
	}

	constexpr float operator""_radf(unsigned long long radians) noexcept {
		return Math::toDegrees(float(radians));
	}


	constexpr double operator""_deg(long double degrees) noexcept {
		return Math::toRadians(double(degrees));
	}

	constexpr double operator""_deg(unsigned long long degrees) noexcept {
		return Math::toRadians(double(degrees));
	}

	constexpr double operator""_rad(long double radians) noexcept {
		return Math::toDegrees(double(radians));
	}

	constexpr double operator""_rad(unsigned long long radians) noexcept {
		return Math::toDegrees(double(radians));
	}


	constexpr long double operator""_degl(long double degrees) noexcept {
		return Math::toRadians(degrees);
	}

	constexpr long double operator""_degl(unsigned long long degrees) noexcept {
		return Math::toRadians(degrees);
	}

	constexpr long double operator""_radl(long double radians) noexcept {
		return Math::toDegrees(radians);
	}

	constexpr long double operator""_radl(unsigned long long radians) noexcept {
		return Math::toDegrees(radians);
	}

}


#ifdef ARC_CFG_MATH_USING_LITERALS
	using namespace Math::Literals;
#endif
