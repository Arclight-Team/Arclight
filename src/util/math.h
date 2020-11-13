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


	constexpr inline double toDegrees(double radians) {
		return radians * 180.0 / pi;
	}

	constexpr inline double toRadians(double degrees) {
		return degrees * pi / 180.0;
	}

	template<Integer I>
	constexpr inline bool isZero(I value) {
		return value == I(0);
	}
	
	template<Float F>
	constexpr inline bool isZero(F value) {
		return Math::abs(value) < Math::epsilon;
	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline bool isEqual(A a, B b) requires (std::is_floating_point_v<A> || std::is_floating_point_v<B>) {

		auto x = Math::abs(a - b);
		auto y = Math::abs(a);
		auto z = Math::abs(b);

		return x <= ((y < z ? z : y) * epsilon);

	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline bool isEqual(A a, B b) requires (std::is_integral_v<A>&& std::is_integral_v<B>) {
		return a == b;
	}

	template<Float F>
	constexpr inline bool isInfinity(F value) {
		return std::isinf(value);
	}

	template<Float F>
	constexpr inline bool isNaN(F value) {
		return std::isnan(value);
	}

	template<Arithmetic A>
	constexpr inline auto sin(A radians) {
		return std::sin(radians);
	}

	template<Arithmetic A>
	constexpr inline auto cos(A radians) {
		return std::cos(radians);
	}

	template<Arithmetic A>
	constexpr inline auto tan(A radians) {
		return std::tan(radians);
	}

	template<Arithmetic A>
	constexpr inline auto asin(A radians) {
		return std::asin(radians);
	}

	template<Arithmetic A>
	constexpr inline auto acos(A radians) {
		return std::acos(radians);
	}

	template<Arithmetic A>
	constexpr inline auto atan(A radians) {
		return std::atan(radians);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline auto atan2(A y, B x) {
		return std::atan2(y, x);
	}

	template<Arithmetic A>
	constexpr inline auto abs(A value) {
		return std::abs(value);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline auto max(A a, B b) {
		return std::max(a, b);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline auto min(A a, B b) {
		return std::min(a, b);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline auto mod(A a, B b) requires (std::is_floating_point_v<A> || std::is_floating_point_v<B>) {
		return std::fmod(a, b);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline auto mod(A a, B b) requires (std::is_integral_v<A> && std::is_integral_v<B>) {
		return a % b;
	}

	template<Arithmetic A>
	constexpr inline auto exp(A exponent) {
		return std::exp(exponent);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline auto pow(A base, B exponent) {
		return std::pow(base, exponent);
	}

	template<Arithmetic A>
	constexpr inline auto ln(A value) {
		return std::log(value);
	}

	template<Arithmetic A>
	constexpr inline auto log(A value) {
		return std::log10(value);
	}

	template<Arithmetic A, Arithmetic B>
	constexpr inline auto log(A base, B value) {
		return Math::log(value) / Math::log(base);
	}

	template<Arithmetic A>
	constexpr inline auto sqrt(A value) {
		return std::sqrt(value);
	}

	template<Arithmetic A>
	constexpr inline auto cbrt(A value) {
		return std::cbrt(value);
	}

	template<Arithmetic A>
	constexpr inline auto ceil(A value) {
		return std::ceil(value);
	}

	template<Arithmetic A>
	constexpr inline auto floor(A value) {
		return std::floor(value);
	}

	template<Arithmetic A>
	constexpr inline auto trunc(A value) {
		return std::trunc(value);
	}

	template<Arithmetic A>
	constexpr inline auto round(A value) {
		return std::round(value);
	}

}