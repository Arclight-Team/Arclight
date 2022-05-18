/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 circle.hpp
 */

#pragma once

#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "arcconfig.hpp"
#include "vector.hpp"
#include "rectangle.hpp"



template<CC::Arithmetic T>
class Circle {
public:

	using Type		= T;

	using ScalarT	= TT::ToFloat<Type>;
	using VecT		= Vec2<Type>;


	// Constructs a unit circle
	constexpr Circle() :
		origin(0),
		radius(0.5f)
	{}

	// Constructs a circle from the radius
	constexpr Circle(T radius, const VecT& origin = VecT()) :
		origin(origin),
		radius(radius)
	{}

	// Constructs a circle from the diameter
	constexpr static Circle fromDiameter(T diameter, const VecT& origin = VecT()) {
		return Circle(diameter / ScalarT(2), origin);
	}

	// Calculates the min point of the circle's bounding box
	constexpr VecT start() const {
		return origin - VecT(radius);
	}

	// Calculates the max point of the circle's bounding box
	constexpr VecT end() const {
		return origin + VecT(radius);
	}

	// Calculates the bounding box of the circle
	constexpr Rectangle<T> boundingBox() const {
		return Rectangle<T>::fromPoints(start(), end());
	}

	// Calculates the surface area of the circle
	constexpr T area() const {
		return Math::pi * radius * radius;
	}

	// Calculates the perimeter of the circle
	constexpr T perimeter() const {
		return Math::pi * radius * ScalarT(2);
	}

	// Calculates the diameter of the circle
	constexpr T diameter() const {
		return radius * ScalarT(2);
	}

	// Checks if the circle intersects with another circle
	template<CC::Arithmetic B>
	constexpr bool intersects(const Circle<B>& other) const {
		return Math::lessEqual(origin.distance(other.origin), radius + other.radius);
	}

	// Checks if the circle contains another circle
	template<CC::Arithmetic B>
	constexpr bool contains(const Circle<B>& other) const {
		return Math::less(origin.distance(other.origin) + other.radius, radius);
	}

	// Checks if the circle contains a point
	template<CC::Arithmetic B>
	constexpr bool contains(const Vec2<B>& point) const {
		return Math::lessEqual(origin.distance(point), radius);
	}

	// Compares two circles
	constexpr bool equal(const Circle<T>& other) const {
		return origin == other.origin && radius == other.radius;
	}

	constexpr bool operator==(const Circle<T>& rhs) const {
		return equal(rhs);
	}

	VecT origin;
	T radius;

};

using CircleB     = Circle<bool>;
using CircleC     = Circle<i8>;
using CircleS     = Circle<i16>;
using CircleI     = Circle<i32>;
using CircleL     = Circle<i64>;
using CircleUC    = Circle<u8>;
using CircleUS    = Circle<u16>;
using CircleUI    = Circle<u32>;
using CircleUL    = Circle<u64>;
using CircleF     = Circle<float>;
using CircleD     = Circle<double>;
using CircleLD    = Circle<long double>;
using CircleX     = Circle<ARC_STD_FLOAT_TYPE>;
