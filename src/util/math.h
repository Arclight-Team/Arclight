#pragma once

#include <type_traits>
#include <cmath>

#include "types.h"


template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<typename T>
concept Float = std::is_floating_point_v<T>;

template<typename T>
concept Integer = std::is_integral_v<T>;


namespace Math {

	constexpr double pi = 3.1415926535897932384626434;
	constexpr double e =  2.7182818284590452353602875;
	constexpr double epsilon = 0.000001;


	constexpr double toDegrees(double radians) {
		return radians * 180.0 / pi;
	}

	constexpr double toRadians(double degrees) {
		return degrees * pi / 180.0;
	}

	template<Arithmetic A>
	constexpr auto abs(A value) {
		return std::abs(value);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto max(A a, B b) {
		return std::max(a, b);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto min(A a, B b) {
		return std::min(a, b);
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
	constexpr bool isEqual(A a, B b) requires (std::is_integral_v<A>&& std::is_integral_v<B>) {
		return a == b;
	}

	template<Float F>
	constexpr bool isInfinity(F value) {
		return std::isinf(value);
	}

	template<Float F>
	constexpr bool isNaN(F value) {
		return std::isnan(value);
	}

	template<Arithmetic A>
	constexpr auto sin(A radians) {
		return std::sin(radians);
	}

	template<Arithmetic A>
	constexpr auto cos(A radians) {
		return std::cos(radians);
	}

	template<Arithmetic A>
	constexpr auto tan(A radians) {
		return std::tan(radians);
	}

	template<Arithmetic A>
	constexpr auto asin(A radians) {
		return std::asin(radians);
	}

	template<Arithmetic A>
	constexpr auto acos(A radians) {
		return std::acos(radians);
	}

	template<Arithmetic A>
	constexpr auto atan(A radians) {
		return std::atan(radians);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto atan2(A y, B x) {
		return std::atan2(y, x);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto mod(A a, B b) requires (std::is_floating_point_v<A> || std::is_floating_point_v<B>) {
		return std::fmod(a, b);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto mod(A a, B b) requires (std::is_integral_v<A> && std::is_integral_v<B>) {
		return a % b;
	}

	template<Arithmetic A>
	constexpr auto exp(A exponent) {
		return std::exp(exponent);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto pow(A base, B exponent) {
		return std::pow(base, exponent);
	}

	template<Arithmetic A>
	constexpr auto ln(A value) {
		return std::log(value);
	}

	template<Arithmetic A>
	constexpr auto log(A value) {
		return std::log10(value);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr auto log(A base, B value) {
		return Math::log(value) / Math::log(base);
	}

	template<Arithmetic A>
	constexpr auto sqrt(A value) {
		return std::sqrt(value);
	}

	template<Arithmetic A>
	constexpr auto cbrt(A value) {
		return std::cbrt(value);
	}

	template<Arithmetic A>
	constexpr auto ceil(A value) {
		return std::ceil(value);
	}

	template<Arithmetic A>
	constexpr auto floor(A value) {
		return std::floor(value);
	}

	template<Arithmetic A>
	constexpr auto trunc(A value) {
		return std::trunc(value);
	}

	template<Arithmetic A>
	constexpr auto round(A value) {
		return std::round(value);
	}

	template<Arithmetic A>
	constexpr auto sign(A value) {
		return (value > A(0)) - (value < A(0));
	}


	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr auto lerp(A start, B end, C factor) {
		return start + factor * (end - start);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr auto clamp(A value, B lowerBound, C upperBound) {
		return value < lowerBound ? lowerBound : (value > upperBound ? upperBound : value);
	}

}