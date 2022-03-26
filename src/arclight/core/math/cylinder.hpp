/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cylinder.hpp
 */

#pragma once

#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "arcconfig.hpp"
#include "vector.hpp"
#include "box.hpp"



template<Arithmetic T>
class Cylinder {
public:

	using Type		= T;

	using ScalarT	= TT::ToFloat<Type>;
	using VecT		= Vec3<Type>;


	// Constructs a unit cylinder
	constexpr Cylinder() :
		origin(0),
		radius(0.5f),
		height(1.0f)
	{}

	// Constructs a cylinder from radius and height
	constexpr Cylinder(T radius, T height = T(1), const VecT& origin = VecT()) :
		origin(origin),
		radius(radius),
		height(height)
	{}

	// Constructs a cylinder from diameter and height
	constexpr Cylinder fromDiameter(T diameter, T height = T(1), const VecT& origin = VecT()) {
		return Cylinder(diameter / ScalarT(2), height, origin);
	}

	// Calculates the min point of the cylinder's bounding box
	constexpr VecT start() const {
		return origin - VecT(radius, height / ScalarT(2), radius);
	}

	// Calculates the max point of the cylinder's bounding box
	constexpr VecT end() const {
		return origin + VecT(radius, height / ScalarT(2), radius);
	}

	// Calculates the bounding box of the cylinder
	constexpr Box<T> boundingBox() const {
		return Box<T>::fromPoints(start(), end());
	}

	// Calculates the surface area of the top/bottom bases
	constexpr T baseArea() const {
		return Math::pi * radius * radius;
	}

	// Calculates the surface area of the side
	constexpr T sideArea() const {
		return ScalarT(2) * Math::pi * radius * height;
	}

	// Calculates the total surface area of the cylinder
	constexpr T area() const {
		return sideArea() + baseArea() * ScalarT(2);
	}

	// Calculate the volume of the cylinder
	constexpr T volume() const {
		return Math::pi * radius * radius * height;
	}

	// Calculates the diameter of the cylinder
	constexpr T diameter() const {
		return radius * ScalarT(2);
	}
	
	// Checks if the cylinder intersects with another cylinder
	template<Arithmetic B>
	constexpr bool intersects(const Cylinder<B>& other) const {
	
		Vec3<T> s1 = start();
		Vec3<T> e1 = end();
		Vec3<B> s2 = other.start();
		Vec3<B> e2 = other.end();

		// Cylinders do not vertically intersect
		if (s2.y >= e1.y || e2.y <= s1.y)
			return false;

		// Calculate intersection on a 2D plane
		s1 = origin;
		s2 = other.origin;
		s1.y = 0;
		s2.y = 0;

		// Circle intersection
		return Math::lessEqual(s1.distance(s2), radius + other.radius);

	}

	// Checks if the cylinder contains another cylinder
	template<Arithmetic B>
	constexpr bool contains(const Cylinder<B>& other) const {

		Vec3<T> s1 = start();
		Vec3<T> e1 = end();
		Vec3<B> s2 = other.start();
		Vec3<B> e2 = other.end();

		// Cylinders do not vertically intersect
		if (s2.y <= s1.y || e1.y <= e2.y)
			return false;

		// Calculate intersection on a 2D plane
		s1 = origin;
		s2 = other.origin;
		s1.y = 0;
		s2.y = 0;

		// Circle intersection
		return Math::less(s1.distance(s2) + other.radius, radius);

	}

	// Checks if the cylinder contains a point
	template<Arithmetic B>
	constexpr bool contains(const Vec3<B>& point) const {

		Vec3<B> p = point;
		Vec3<T> s = start();
		Vec3<T> e = end();

		// Point does not vertically intersect
		if (!Math::inRange(p.y, s.y, e.y))
			return false;

		// Calculate intersection on a 2D plane
		s = origin;
		s.y = 0;
		p.y = 0;

		// Circle intersection
		return Math::lessEqual(s.distance(p), radius);

	}

	// Compares two cylinders
	constexpr bool equal(const Cylinder<T>& other) const {
		return origin == other.origin && radius == other.radius && height == other.height;
	}

	constexpr bool operator==(const Cylinder<T>& rhs) const {
		return equal(rhs);
	}

	VecT origin;
	T radius;
	T height;

};

using CylinderB     = Cylinder<bool>;
using CylinderC     = Cylinder<i8>;
using CylinderS     = Cylinder<i16>;
using CylinderI     = Cylinder<i32>;
using CylinderL     = Cylinder<i64>;
using CylinderUC    = Cylinder<u8>;
using CylinderUS    = Cylinder<u16>;
using CylinderUI    = Cylinder<u32>;
using CylinderUL    = Cylinder<u64>;
using CylinderF     = Cylinder<float>;
using CylinderD     = Cylinder<double>;
using CylinderLD    = Cylinder<long double>;
using CylinderX     = Cylinder<ARC_STD_FLOAT_TYPE>;
