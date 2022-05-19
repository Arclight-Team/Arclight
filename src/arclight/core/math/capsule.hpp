/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 capsule.hpp
 */

#pragma once

#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "arcconfig.hpp"
#include "vector.hpp"
#include "box.hpp"
#include "cylinder.hpp"
#include "sphere.hpp"



template<CC::Arithmetic T>
class Capsule {
public:

	using Type		= T;

	using ScalarT	= TT::ToFloat<Type>;
	using VecT		= Vec3<Type>;


	// Constructs a unit capsule
	constexpr Capsule() :
		origin(0),
		radius(0.5f),
		height(1.0f)
	{}

	// Constructs a capsule from radius and height
	constexpr Capsule(T radius, T height = T(1), const VecT& origin = VecT()) :
		origin(origin),
		radius(radius),
		height(height)
	{
		arc_assert(height >= T(1), "Capsule height cannot be less than one");
		arc_assert(radius >  Zero, "Capsule radius cannot be less than or equal to zero");
	}

	// Constructs a capsule from diameter and height
	constexpr static Capsule fromDiameter(T diameter, T height = T(1), const VecT& origin = VecT()) {
		return Capsule(diameter / ScalarT(2), height, origin);
	}

	// Calculates the min point of the capsule's bounding box
	constexpr VecT start() const {
		return origin - VecT(radius, height / ScalarT(2), radius);
	}

	// Calculates the max point of the capsule's bounding box
	constexpr VecT end() const {
		return origin + VecT(radius, height / ScalarT(2), radius);
	}

	// Calculates the bounding box of the capsule
	constexpr Box<T> boundingBox() const {
		return Box<T>::fromPoints(start(), end());
	}

	// Returns a sphere representing the capsule's top hemisphere
	constexpr Sphere<T> topHemisphere() const {
		return Sphere<T>(radius, { origin.x, origin.y + (height / ScalarT(2)) - radius, origin.z });
	}

	// Returns a sphere representing the capsule's bottom hemisphere
	constexpr Sphere<T> bottomHemisphere() const {
		return Sphere<T>(radius, { origin.x, origin.y - (height / ScalarT(2)) - radius, origin.z });
	}

	// Returns a cylinder representing the capsule's body
	constexpr Cylinder<T> cylinder() const {
		return Cylinder<T>(radius, height / ScalarT(2), origin);
	}

	// Calculates the surface area of the top/bottom hemispheres
	constexpr T hemisphereArea() const {
		return Math::pi * radius * radius;
	}

	// Calculates the surface area of the side
	constexpr T sideArea() const {
		return ScalarT(2) * Math::pi * radius * height;
	}

	// Calculates the total surface area of the capsule
	constexpr T area() const {
		return sideArea() + hemisphereArea() * ScalarT(2);
	}

	// Calculate the volume of the capsule
	constexpr T volume() const {
		return Math::pi * radius * radius * height;
	}

	// Calculates the diameter of the capsule
	constexpr T diameter() const {
		return radius * ScalarT(2);
	}
	
	// Checks if the capsule intersects with another capsule
	template<CC::Arithmetic B>
	constexpr bool intersects(const Capsule<B>& other) const {

		static_assert(false, "Incomplete or missing functionality");

		// TODO: Check cylinder-top hemisphere intersection

		// TODO: Check cylinder-bottom hemisphere intersection

		// Check cylinder-cylinder intersection
		if (cylinder().intersects(other.cylinder()))
			return true;

		// Check top hemisphere-top hemisphere intersection
		if (topHemisphere().intersects(other.topHemisphere()))
			return true;

		// Check top hemisphere-bottom hemisphere intersection
		if (topHemisphere().intersects(other.bottomHemisphere()))
			return true;

		// TODO: Check top hemisphere-cylinder intersection

		// Check bottom hemisphere-top hemisphere intersection
		if (bottomHemisphere().intersects(other.topHemisphere()))
			return true;
		
		// Check bottom hemisphere-bottom hemisphere intersection
		if (bottomHemisphere().intersects(other.bottomHemisphere()))
			return true;

		// TODO: Check bottom hemisphere-cylinder intersection

		return false;
	}

	// Checks if the capsule contains another capsule
	template<CC::Arithmetic B>
	constexpr bool contains(const Capsule<B>& other) const {

		static_assert(false, "Incomplete or missing functionality");

		// TODO: figure out a clean to do this
		return false;

	}

	// Checks if the capsule contains a point
	template<CC::Arithmetic B>
	constexpr bool contains(const Vec3<B>& point) const {

		if (topHemisphere().contains(point))
			return true;

		if (bottomHemisphere().contains(point))
			return true;

		return cylinder().contains(point);

	}

	// Compares two capsules
	constexpr bool equal(const Capsule<T>& other) const {
		return origin == other.origin && radius == other.radius && height == other.height;
	}

	constexpr bool operator==(const Capsule<T>& rhs) const {
		return equal(rhs);
	}

	VecT origin;
	T radius;
	T height;

};

using CapsuleB     = Capsule<bool>;
using CapsuleC     = Capsule<i8>;
using CapsuleS     = Capsule<i16>;
using CapsuleI     = Capsule<i32>;
using CapsuleL     = Capsule<i64>;
using CapsuleUC    = Capsule<u8>;
using CapsuleUS    = Capsule<u16>;
using CapsuleUI    = Capsule<u32>;
using CapsuleUL    = Capsule<u64>;
using CapsuleF     = Capsule<float>;
using CapsuleD     = Capsule<double>;
using CapsuleLD    = Capsule<long double>;
using CapsuleX     = Capsule<ARC_STD_FLOAT_TYPE>;
