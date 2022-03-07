/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 sphere.hpp
 */

#pragma once

#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "arcconfig.hpp"
#include "vector.hpp"
#include "box.hpp"



template<Arithmetic T>
class Sphere
{
public:

	using Type		= T;
	
	using ScalarT	= TT::ToFloat<Type>;
	using VecT		= Vec3<Type>;


	// Constructs a unit sphere
	constexpr Sphere() :
		origin(0),
		radius(0.5f)
	{}

	// Constructs a circle from the radius
	constexpr Sphere(T radius, const VecT& origin = VecT()) :
		origin(origin),
		radius(radius)
	{}

	// Constructs a circle from the diameter
	constexpr static Sphere fromDiameter(T diameter, const VecT& origin = VecT()) {
		return Sphere(diameter / ScalarT(2), origin);
	}

	// Calculates the min point of the sphere's bounding box
	constexpr VecT start() const {
		return origin - VecT(radius);
	}

	// Calculates the max point of the sphere's bounding box
	constexpr VecT end() const {
		return origin + VecT(radius);
	}

	// Calculates the bounding box of the sphere
	constexpr Box<T> boundingBox() const {
		return Box<T>::fromPoints(start(), end());
	}

	// Calculates the total surface area of the sphere
	constexpr T area() const {
		return ScalarT(4) * Math::pi * radius * radius;
	}

	// Calculate the volume of the sphere
	constexpr T volume() const {
		return (ScalarT(4) * Math::pi * radius * radius * radius) / ScalarT(3);
	}

	// Calculates the diameter of the sphere
	constexpr T diameter() const {
		return radius * ScalarT(2);
	}

	// Checks if the sphere intersects with another sphere
	template<Arithmetic B>
	constexpr bool intersects(const Sphere<B>& other) const {
		return Math::lessEqual(origin.distance(other.origin), radius + other.radius);
	}

	// Checks if the sphere contains another sphere
	template<Arithmetic B>
	constexpr bool contains(const Sphere<B>& other) const {
		return Math::less(origin.distance(other.origin) + other.radius, radius);
	}

	// Checks if the sphere contains a point
	template<Arithmetic B>
	constexpr bool contains(const Vec3<B>& point) const {
		return Math::lessEqual(origin.distance(point), radius);
	}

	// Compares two spheres
	constexpr bool equal(const Sphere<T>& other) const {
		return origin == other.origin && radius == other.radius;
	}

	constexpr bool operator==(const Sphere<T>& rhs) const {
		return equal(rhs);
	}

	VecT origin;
	T radius;

};

using SphereB     = Sphere<bool>;
using SphereC     = Sphere<i8>;
using SphereS     = Sphere<i16>;
using SphereI     = Sphere<i32>;
using SphereL     = Sphere<i64>;
using SphereUC    = Sphere<u8>;
using SphereUS    = Sphere<u16>;
using SphereUI    = Sphere<u32>;
using SphereUL    = Sphere<u64>;
using SphereF     = Sphere<float>;
using SphereD     = Sphere<double>;
using SphereLD    = Sphere<long double>;
using SphereX     = Sphere<ARC_STD_FLOAT_TYPE>;
