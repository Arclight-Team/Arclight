#pragma once

#include <cmath>
#include "common/types.h"


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
		return std::max(a, b);
	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic... Args>
	constexpr auto min(A a, B b, C c, Args... args) {
		return std::min({ a, b, c, args... });
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
	constexpr bool isEqual(A a, B b) requires (std::is_integral_v<A> && std::is_integral_v<B>) {
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
	constexpr auto round(A value, u32 digits) noexcept {
		return (value * Math::pow(10, digits) + 0.5) / Math::pow(10, digits);
	}

	template<Arithmetic A>
	constexpr auto sign(A value) noexcept {
		return (value > A(0)) - (value < A(0));
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

	constexpr bool isAligned(AddressT x, AlignT alignment) noexcept {
		return !(x & (alignment - 1));
	}

	constexpr AddressT alignUp(AddressT x, AlignT alignment) noexcept {
		return (x + alignment - 1) & ~(alignment - 1);
	}

	constexpr AddressT alignDown(AddressT x, AlignT alignment) noexcept {
		return x & ~(alignment - 1);
	}

	template<class T>
	constexpr AddressT address(T* ptr) {
		return reinterpret_cast<AddressT>(ptr);
	}

}
