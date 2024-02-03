/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 sphere.hpp
 */

#pragma once

#include "shape.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "arcconfig.hpp"
#include "../vector.hpp"
#include "box.hpp"



template<CC::Arithmetic A>
class Sphere : public IShape {

public:

	using Type = A;
	
	using ScalarT = TT::ToFloat<Type>;
	using VecT = Vec3<Type>;


	constexpr Sphere() noexcept : Sphere(A(0.5f)) {}

	explicit constexpr Sphere(A radius, const VecT& origin = {}) noexcept : origin(origin), radius(radius) {}

	constexpr Sphere(A x, A y, A z, A r) noexcept : Sphere(r, {x, y, z}) {}

	template<CC::Arithmetic B>
	constexpr Sphere(const Sphere<B>& sphere) noexcept : Sphere(sphere.radius, sphere.origin) {}


	constexpr static Sphere fromDiameter(A diameter, const VecT& origin = {}) noexcept {
		return {diameter / 2, origin};
	}


	constexpr bool valid() noexcept {
		return Math::greater(radius, 0);
	}

	constexpr VecT start() const noexcept {
		return origin - VecT(radius);
	}

	constexpr VecT end() const noexcept {
		return origin + VecT(radius);
	}
	
	constexpr Box<A> bounds() const noexcept {
		return Box<A>::fromPoints(start(), end());
	}


	constexpr ScalarT area() const noexcept {
		return 4 * Math::TPi<ScalarT> * radius * radius;
	}

	constexpr ScalarT volume() const noexcept {
		return (4 * Math::TPi<ScalarT> * radius * radius * radius) / 3;
	}

	constexpr A diameter() const noexcept {
		return radius * 2;
	}


	template<CC::Arithmetic B>
	constexpr bool intersects(const Sphere<B>& other) const {
		return Math::lessEqual(origin.distance(other.origin), radius + other.radius);
	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Sphere<B>& other) const {
		return Math::less(origin.distance(other.origin) + other.radius, radius);
	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Vec3<B>& point) const {
		return Math::lessEqual(origin.distance(point), radius);
	}


	template<CC::Arithmetic B>
	constexpr bool operator==(const Sphere<B>& other) const {
		return origin == other.origin && Math::equal(radius, other.radius);
	}


	VecT origin;
	A radius;

};


template<CC::Arithmetic A>
RawLog& operator<<(RawLog& log, const Sphere<A>& sphere) {

	log << "Sphere[";
	log << "[" << sphere.origin.x << ", " << sphere.origin.y << ", " << sphere.origin.z << "], ";
	log << "R = " << sphere.radius << "]";

	return log;

}


ARC_DEFINE_ARITHMETIC_ALIASES(1, Sphere, Sphere)
