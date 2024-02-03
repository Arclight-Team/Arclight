/*
 * Copyright (c) 2022 - Arclight Team
 *
 * This file is part of Arclight. All rights reserved.
 *
 * circle.hpp
 */

#pragma once

#include "shape.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "arcconfig.hpp"
#include "../vector.hpp"
#include "rectangle.hpp"



template<CC::Arithmetic A>
class Circle : public IShape {

public:

	using Type = A;

	using ScalarT = TT::ToFloat<Type>;
	using VecT = Vec2<Type>;


	constexpr Circle() noexcept : Circle(A(0.5)) {}

	explicit constexpr Circle(A radius, const VecT& origin = {}) noexcept : origin(origin), radius(radius) {}

	constexpr Circle(A x, A y, A r) noexcept : Circle(r, {x, y}) {}

	template<CC::Arithmetic B>
	constexpr Circle(const Circle<B>& circle) noexcept : Circle(circle.radius, circle.origin) {}


	constexpr static Circle fromDiameter(A diameter, const VecT& origin = {}) noexcept {
		return {diameter / 2, origin};
	}


	constexpr bool valid() noexcept {
		return Math::greater(0, radius);
	}

	constexpr VecT start() const noexcept {
		return origin - VecT(radius);
	}

	constexpr VecT end() const noexcept {
		return origin + VecT(radius);
	}

	constexpr Rectangle<A> bounds() const noexcept {
		return Rectangle<A>::fromPoints(start(), end());
	}


	constexpr ScalarT area() const noexcept {
		return Math::TPi<ScalarT> * radius * radius;
	}

	constexpr ScalarT perimeter() const noexcept {
		return Math::TPi<ScalarT> * radius * 2;
	}

	constexpr A diameter() const noexcept {
		return radius * 2;
	}


	template<CC::Arithmetic B>
	constexpr bool intersects(const Circle<B>& other) const {
		return Math::lessEqual(origin.distance(other.origin), radius + other.radius);
	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Circle<B>& other) const {
		return Math::less(origin.distance(other.origin) + other.radius, radius);
	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Vec2<B>& point) const {
		return Math::lessEqual(origin.distance(point), radius);
	}


	template<CC::Arithmetic B>
	constexpr bool operator==(const Circle<B>& other) const {
		return (origin == other.origin) && Math::equal(radius, other.radius);
	}


	VecT origin;
	A radius;

};


template<CC::Arithmetic A>
RawLog& operator<<(RawLog& log, const Circle<A>& circle) {

	log << "Circle[";
	log << "[" << circle.origin.x << ", " << circle.origin.y << "], ";
	log << "R = " << circle.radius << "]";

	return log;

}


ARC_DEFINE_ARITHMETIC_ALIASES(1, Circle, Circle)
