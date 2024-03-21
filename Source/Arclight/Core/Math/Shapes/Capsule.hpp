/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Capsule.hpp
 */

#pragma once

#include "Shape.hpp"
#include "Meta/Concepts.hpp"
#include "Meta/TypeTraits.hpp"
#include "Common/Config.hpp"
#include "Math/Vector.hpp"
#include "Box.hpp"
#include "Cylinder.hpp"
#include "Sphere.hpp"
#include "Common/Assert.hpp"



template<CC::Arithmetic A>
class Capsule : public IShape {

public:

	using Type = A;

	using ScalarT = TT::ToFloat<Type>;
	using VecT = Vec3<Type>;


	constexpr Capsule() noexcept : Capsule(A(0.5), A(1.0)) {}

	explicit constexpr Capsule(A radius, A height = {1}, const VecT& origin = {}) noexcept : origin(origin), radius(radius), height(height) {}

	constexpr Capsule(A x, A y, A z, A r, A h) noexcept : Capsule(r, h, {x, y, z}) {}
	
	template<CC::Arithmetic B>
	constexpr Capsule(const Capsule<B>& capsule) noexcept : Capsule(capsule.radius, capsule.height, capsule.origin) {}


	constexpr static Capsule fromDiameter(A diameter, A height = {1}, const VecT& origin = {}) noexcept {
		return {diameter / 2, height, origin};
	}


	constexpr bool valid() noexcept {
		return Math::greater(radius, 0) && Math::greaterEqual(height, 1);
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

	constexpr Sphere<A> topHemisphere() const noexcept {
		return {radius, { origin.x, origin.y + (height / 2) - radius, origin.z }};
	}

	constexpr Sphere<A> bottomHemisphere() const noexcept {
		return {radius, { origin.x, origin.y - (height / 2) - radius, origin.z }};
	}

	constexpr Cylinder<A> cylinder() const noexcept {
		return {radius, height / 2, origin};
	}


	constexpr ScalarT hemisphereArea() const noexcept {
		return Math::TPi<ScalarT> * radius * radius;
	}

	constexpr ScalarT sideArea() const noexcept {
		return 2 * Math::TPi<ScalarT> * radius * height;
	}

	constexpr ScalarT area() const noexcept {
		return sideArea() + hemisphereArea() * 2;
	}

	constexpr ScalarT volume() const noexcept {
		return Math::TPi<ScalarT> * radius * radius * height;
	}

	constexpr A diameter() const noexcept {
		return radius * 2;
	}


	template<CC::Arithmetic B>
	constexpr bool intersects(const Capsule<B>& other) const {

		static_assert(TT::False<B>, "Incomplete or missing functionality");

		// TODO Cylinder intersection with top/bottom hemisphere and vice versa

		return {
			cylinder().intersects(other.cylinder()) ||
			topHemisphere().intersects(other.topHemisphere()) ||
			topHemisphere().intersects(other.bottomHemisphere()) ||
			bottomHemisphere().intersects(other.topHemisphere()) ||
			bottomHemisphere().intersects(other.bottomHemisphere())
		};

	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Capsule<B>& other) const {

		static_assert(TT::False<B>, "Incomplete or missing functionality");

		// TODO Implement
		return false;

	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Vec3<B>& point) const {

		return {
			topHemisphere().contains(point) ||
			bottomHemisphere().contains(point) ||
			cylinder().contains(point)
		};

	}


	template<CC::Arithmetic B>
	constexpr bool operator==(const Capsule<B>& other) const {
		return origin == other.origin && Math::equal(radius, other.radius) && Math::equal(height, other.height);
	}


	VecT origin;
	A radius;
	A height;

};


template<CC::Arithmetic A>
inline RawLog& operator<<(RawLog& log, const Capsule<A>& cap) {

	log << "Capsule[";
	log << "[" << +cap.origin.x << ", " << cap.origin.y << ", " << cap.origin.z << "], ";
	log << "R = " << cap.radius << ", H = " << cap.height << "]";

	return log;

}


ARC_DEFINE_ARITHMETIC_ALIASES(1, Capsule, Capsule)
