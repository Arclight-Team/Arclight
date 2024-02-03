/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cylinder.hpp
 */

#pragma once

#include "shape.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "arcconfig.hpp"
#include "../vector.hpp"
#include "box.hpp"



template<CC::Arithmetic A>
class Cylinder : public IShape {

public:

	using Type = A;

	using ScalarT = TT::ToFloat<Type>;
	using VecT = Vec3<Type>;


	constexpr Cylinder() noexcept : Cylinder(A(0.5), A(1.0)) {}

	explicit constexpr Cylinder(A radius, A height = {1}, const VecT& origin = {}) noexcept : origin(origin), radius(radius), height(height) {}

	constexpr Cylinder(A x, A y, A z, A r, A h) noexcept : Cylinder(r, h, {x, y, z}) {}

	template<CC::Arithmetic B>
	constexpr Cylinder(const Cylinder<B>& cylinder) noexcept : Cylinder(cylinder.radius, cylinder.height, cylinder.origin) {}


	constexpr static Cylinder fromDiameter(A diameter, A height = {1}, const VecT& origin = {}) noexcept {
		return {diameter / 2, height, origin};
	}


	constexpr bool valid() noexcept {
		return Math::greater(radius, 0) && Math::greater(height, 0);
	}

	constexpr VecT start() const noexcept {
		return origin - VecT(radius, height / 2, radius);
	}

	constexpr VecT end() const noexcept {
		return origin + VecT(radius, height / 2, radius);
	}

	constexpr Box<A> bounds() const noexcept {
		return Box<A>::fromPoints(start(), end());
	}


	constexpr ScalarT baseArea() const noexcept {
		return Math::TPi<ScalarT> * radius * radius;
	}

	constexpr ScalarT sideArea() const noexcept {
		return 2 * Math::TPi<ScalarT> * radius * height;
	}

	constexpr ScalarT area() const noexcept {
		return sideArea() + baseArea() * 2;
	}

	constexpr ScalarT volume() const noexcept {
		return Math::TPi<ScalarT> * radius * radius * height;
	}

	constexpr A diameter() const {
		return radius * 2;
	}


	template<CC::Arithmetic B>
	__ARC_CMATH26 bool intersects(const Cylinder<B>& other) const {
	
		Vec3<A> s1 = start();
		Vec3<A> e1 = end();
		Vec3<B> s2 = other.start();
		Vec3<B> e2 = other.end();

		// Cylinders do not vertically intersect
		if (s2.y >= e1.y || e2.y <= s1.y) {
			return false;
		}

		// Calculate intersection on a 2D plane
		s1 = origin;
		s2 = other.origin;
		s1.y = 0;
		s2.y = 0;

		// Circle intersection
		return Math::lessEqual(s1.distance(s2), radius + other.radius);

	}

	template<CC::Arithmetic B>
	__ARC_CMATH26 bool contains(const Cylinder<B>& other) const {

		Vec3<A> s1 = start();
		Vec3<A> e1 = end();
		Vec3<B> s2 = other.start();
		Vec3<B> e2 = other.end();

		// Cylinders do not vertically intersect
		if (s2.y <= s1.y || e1.y <= e2.y) {
			return false;
		}

		// Calculate intersection on a 2D plane
		s1 = origin;
		s2 = other.origin;
		s1.y = 0;
		s2.y = 0;

		// Circle intersection
		return Math::less(s1.distance(s2) + other.radius, radius);

	}

	template<CC::Arithmetic B>
	__ARC_CMATH26 bool contains(const Vec3<B>& point) const {

		Vec3<B> p = point;
		Vec3<A> s = start();
		Vec3<A> e = end();

		// Point does not vertically intersect
		if (!Math::inRange(p.y, s.y, e.y)) {
			return false;
		}

		// Calculate intersection on a 2D plane
		s = origin;
		s.y = 0;
		p.y = 0;

		// Circle intersection
		return Math::lessEqual(s.distance(p), radius);

	}


	template<CC::Arithmetic B>
	constexpr bool operator==(const Cylinder<A>& other) const {
		return origin == other.origin && Math::equal(radius, other.radius) && Math::equal(height, other.height);
	}


	VecT origin;
	A radius;
	A height;

};


template<CC::Arithmetic A>
RawLog& operator<<(RawLog& log, const Cylinder<A>& cylinder) {

	log << "Cylinder[";
	log << "[" << cylinder.origin.x << ", " << cylinder.origin.y << ", " << cylinder.origin.z << "], ";
	log << "R = " << cylinder.radius << ", H = " << cylinder.height << "]";

	return log;

}


ARC_DEFINE_ARITHMETIC_ALIASES(1, Cylinder, Cylinder)
