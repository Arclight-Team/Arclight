#pragma once

#include <cmath>
#include "types.h"
#include "util/concepts.h"


#if defined(ARC_CMATH_CONSTEXPR_FIX) && ARC_CMATH_CONSTEXPR_FIX
#define ARC_CMATH_CONSTEXPR constexpr
#else
#define ARC_CMATH_CONSTEXPR __forceinline
#endif


namespace Math {

	constexpr double pi = 3.1415926535897932384626434;
	constexpr double e =  2.7182818284590452353602875;
	constexpr double epsilon = 0.000001;


	constexpr double toDegrees(double radians) noexcept {
		return radians * 180.0 / pi;
	}

	constexpr double toRadians(double degrees) noexcept {
		return degrees * pi / 180.0;
	}

	template<Arithmetic A>
	constexpr auto abs(A value) {
		return std::abs(value);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic... Args>
	constexpr auto max(A a, B b, C c, Args... args) {
		return std::max({ a, b, c, args... });
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto max(A a, B b) {
		return std::max<std::common_type_t<A, B>>(a, b);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic... Args>
	constexpr auto min(A a, B b, C c, Args... args) {
		return std::min({ a, b, c, args... });
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto min(A a, B b) {
		return std::min<std::common_type_t<A, B>>(a, b);
	}

	template<class T>
	constexpr auto ascOrder(T& a, T& b) {

		if(b < a) {
			std::swap(a, b);
		}

	}

	template<class T>
	constexpr auto descOrder(T& a, T& b) {

		if(a < b) {
			std::swap(a, b);
		}

	}

	template<Integer I>
	constexpr bool isZero(I value) {
		return value == I(0);
	}

	template<Float F>
	constexpr bool isZero(F value) {
		return Math::abs(value) < Math::epsilon;
	}

	template<Arithmetic A, Arithmetic B>
	constexpr bool isEqual(A a, B b) requires (std::is_floating_point_v<A> || std::is_floating_point_v<B>) {

		auto x = Math::abs(a - b);
		auto y = Math::abs(a);
		auto z = Math::abs(b);

		return x <= (Math::max(y, z) * epsilon);

	}

	template<Arithmetic A, Arithmetic B>
	constexpr bool isEqual(A a, B b) requires (std::is_integral_v<A> && std::is_integral_v<B>) {
		return a == b;
	}

	template<Float F>
	ARC_CMATH_CONSTEXPR bool isInfinity(F value) {
		return std::isinf(value);
	}

	template<Float F>
	ARC_CMATH_CONSTEXPR bool isNaN(F value) {
		return std::isnan(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto sin(A radians) {
		return std::sin(radians);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto cos(A radians) {
		return std::cos(radians);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto tan(A radians) {
		return std::tan(radians);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto asin(A value) {
		return std::asin(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto acos(A value) {
		return std::acos(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto atan(A value) {
		return std::atan(value);
	}

	template<Arithmetic A, Arithmetic B>
	ARC_CMATH_CONSTEXPR auto atan2(A y, B x) {
		return std::atan2(y, x);
	}

	template<Arithmetic A, Arithmetic B>
	ARC_CMATH_CONSTEXPR auto mod(A a, B b) {
		return std::fmod(a, b);
	}

	template<Integer A, Integer B>
	constexpr auto mod(A a, B b) {
		return a % b;
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto exp(A exponent) {
		return std::exp(exponent);
	}

	template<Arithmetic A, Arithmetic B>
	ARC_CMATH_CONSTEXPR auto pow(A base, B exponent) {
		return std::pow(base, exponent);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto ln(A value) {
		return std::log(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto log(A value) {
		return std::log10(value);
	}

	template<Arithmetic A, Arithmetic B>
	ARC_CMATH_CONSTEXPR auto log(A base, B value) {
		return Math::log(value) / Math::log(base);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto sqrt(A value) {
		return std::sqrt(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto cbrt(A value) {
		return std::cbrt(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto ceil(A value) {
		return std::ceil(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto floor(A value) {
		return std::floor(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto trunc(A value) {
		return std::trunc(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto round(A value) {
		return std::round(value);
	}

	template<Arithmetic A>
	ARC_CMATH_CONSTEXPR auto round(A value, u32 digits) noexcept {
		return (value * Math::pow(10, digits) + 0.5) / Math::pow(10, digits);
	}

	template<Arithmetic A>
	constexpr auto copysign(A value, A sgn) noexcept {
		return std::copysign(value, sgn);
	}

	template<Integer I>
	constexpr auto sign(I value) noexcept {
		return (value > I(0)) - (value < I(0));
	}

	template<Float F>
	constexpr auto sign(F value) noexcept {
		return 1 - std::signbit(value) * 2;
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D, Arithmetic E>
	constexpr auto map(A value, B start1, C end1, D start2, E end2) noexcept {
		return start2 + static_cast<double>(end2 - start2) * (static_cast<double>(value - start1) / static_cast<double>(end1 - start1));
	};

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr auto lerp(A start, B end, C factor) noexcept {
		return start + factor * (end - start);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr auto clamp(A value, B lowerBound, C upperBound) noexcept {
		return value < lowerBound ? lowerBound : (value > upperBound ? upperBound : value);
	}

	template<Integer I>
	constexpr bool isAligned(I x, AlignT alignment) noexcept {
		return !(x & (alignment - 1));
	}

	template<Integer I>
	constexpr I alignUp(I x, AlignT alignment) noexcept {
		return (x + alignment - 1) & ~(alignment - 1);
	}

	template<Integer I>
	constexpr I alignDown(I x, AlignT alignment) noexcept {
		return x & ~(alignment - 1);
	}

	template<class T>
	constexpr AddressT address(T* ptr) {
		return reinterpret_cast<AddressT>(ptr);
	}

}
