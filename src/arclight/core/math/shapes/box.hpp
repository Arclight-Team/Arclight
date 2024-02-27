/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 box.hpp
 */

#pragma once

#include "shape.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "arcconfig.hpp"
#include "../vector.hpp"



template<CC::Arithmetic A>
class Box : public IShape {

public:

	using Type = A;

	using ScalarT = TT::ToFloat<Type>;
	using VecT = Vec3<Type>;


	constexpr Box() noexcept : Box(VecT(1)) {}

	explicit constexpr Box(const VecT& size, const VecT& origin = {}) noexcept : origin(origin), size(size) {}

	constexpr Box(A x, A y, A z, A w, A h, A d) noexcept : Box({w, h, d}, {x, y, z}) {}

	template<CC::Arithmetic B>
	constexpr Box(const Box<B>& box) noexcept : Box(box.size, box.origin) {}


	constexpr static Box fromPoints(const VecT& start, const VecT& end) noexcept {
		return {end - start, (start + end) / 2};
	}


	constexpr bool valid() noexcept {
		return !size.anyNegative();
	}

	constexpr VecT start() const noexcept {
		return origin - size / 2;
	}

	constexpr VecT end() const noexcept {
		return origin + size / 2;
	}

	constexpr Box bounds() const noexcept {
		return *this;
	}


	constexpr A area() const noexcept {
		return 2 * (size.x * size.y + size.x * size.z + size.y * size.z);
	}

	constexpr A volume() const noexcept {
		return size.x * size.y * size.z;
	}

	constexpr A width() const noexcept {
		return size.x;
	}

	constexpr A height() const noexcept {
		return size.y;
	}

	constexpr A depth() const noexcept {
		return size.z;
	}

	constexpr A diagonalSquared() const noexcept {
		return size.x * size.x + size.y * size.y + size.z * size.z;
	}

	ARC_CONSTEXPR_CMATH26 ScalarT diagonal() const {
		return Math::sqrt<ScalarT>(diagonalSquared());
	}


	template<CC::Arithmetic B>
	constexpr bool intersects(const Box<B>& other) const noexcept {

		VecT s1 = start();
		VecT e1 = end();
		Vec3<B> s2 = other.start();
		Vec3<B> e2 = other.end();

		return {
			s2.x < e1.x && e2.x > s1.x &&
			s2.y < e1.y && e2.y > s1.y &&
			s2.z < e1.z && e2.z > s1.z
		};

	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Box<B>& other) const noexcept {

		VecT s1 = start();
		VecT e1 = end();
		Vec3<B> s2 = other.start();
		Vec3<B> e2 = other.end();

		return {
			s2.x > s1.x && e1.x > e2.x &&
			s2.y > s1.y && e1.y > e2.y &&
			s2.z > s1.z && e1.z > e2.z
		};

	}

	template<CC::Arithmetic B>
	constexpr bool contains(const Vec3<B>& point) const noexcept {
		return Math::inRange(point, start(), end());
	}


	template<CC::Arithmetic B>
	constexpr bool operator==(const Box<B>& other) const {
		return origin == other.origin && size == other.size;
	}


	VecT origin;
	VecT size;

};


template<CC::Arithmetic A>
RawLog& operator<<(RawLog& log, const Box<A>& box) {

	log << "Box[";
	log << "[" << box.origin.x << ", " << box.origin.y << ", " << box.origin.z << "], ";
	log << "[" << box.size.x << ", " << box.size.y << ", " << box.size.z << "]]";

	return log;

}


ARC_DEFINE_ARITHMETIC_ALIASES(1, Box, Box)
