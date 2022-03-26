/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 box.hpp
 */

#pragma once

#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "arcconfig.hpp"
#include "vector.hpp"



template<Arithmetic T>
class Box {
public:

	using Type		= T;

	using ScalarT	= TT::ToFloat<Type>;
	using VecT		= Vec3<Type>;


	// Constructs a unit box (cube)
	constexpr Box() :
		origin(0),
		size(1)
	{}

	// Constructs a box from size and origin
	constexpr Box(const VecT& size, const VecT& origin = VecT()) :
		origin(origin),
		size(size)
	{}

	// Constructs a box given two points
	constexpr static Box<T> fromPoints(const VecT& start, const VecT& end) {
		return Box<T>(end - start, (start + end) / 2);
	}

	// Calculates the min point of the box's bounding box
	constexpr VecT start() const {
		return origin - size / 2;
	}

	// Calculates the max point of the box's bounding box
	constexpr VecT end() const {
		return origin + size / 2;
	}

	// Calculates the bounding box of the box
	constexpr Box<T> boundingBox() const {
		return Box<T>::fromPoints(start(), end());
	}

	// Calculates the surface area of the box
	constexpr T area() const {
		return 2 * (size.x * size.y + size.x * size.z + size.y * size.z);
	}

	// Calculates the volume of the box
	constexpr T volume() const {
		return size.x * size.y * size.z;
	}

	// Calculates the squared diagonal of the box
	constexpr T diagonalSquared() const {
		return size.x * size.x + size.y * size.y + size.z * size.z;
	}

	// Calculates the diagonal of the box
	ARC_CMATH_CONSTEXPR T diagonal() const {
		return Math::sqrt(diagonalSquared());
	}

	// Returns the width of the box
	constexpr T width() const {
		return size.x;
	}

	// Returns the height of the box
	constexpr T height() const {
		return size.y;
	}

	// Returns the depth of the box
	constexpr T depth() const {
		return size.z;
	}

	// Checks if the box intersects with another box
	template<Arithmetic B>
	constexpr bool intersects(const Box<B>& other) const {

		Vec3<T> s1 = start();
		Vec3<T> e1 = end();
		Vec3<B> s2 = other.start();
		Vec3<B> e2 = other.end();

		return
			s2.x < e1.x && e2.x > s1.x && 
			s2.y < e1.y && e2.y > s1.y && 
			s2.z < e1.z && e2.z > s1.z;

	}

	// Checks if the box contains another box
	template<Arithmetic B>
	constexpr bool contains(const Box<B>& other) const {

		Vec3<T> s1 = start();
		Vec3<T> e1 = end();
		Vec3<B> s2 = other.start();
		Vec3<B> e2 = other.end();

		return
			s2.x > s1.x && e1.x > e2.x &&
			s2.y > s1.y && e1.y > e2.y &&
			s2.z > s1.z && e1.z > e2.z;

	}

	// Checks if the box contains a point
	template<Arithmetic B>
	constexpr bool contains(const Vec3<B>& point) const {

		Vec3<T> s = start();
		Vec3<T> e = end();

		// TODO: Math::inRange() vector overload
		return
			Math::inRange(point.x, s.x, e.x) &&
			Math::inRange(point.y, s.y, e.y) &&
			Math::inRange(point.z, s.z, e.z);

	}

	// Compares two boxes
	constexpr bool equal(const Box<T>& other) const {
		return origin == other.origin && size == other.size;
	}

	constexpr bool operator==(const Box<T>& rhs) const {
		return equal(rhs);
	}

	VecT origin;
	VecT size;

};

using BoxB     = Box<bool>;
using BoxC     = Box<i8>;
using BoxS     = Box<i16>;
using BoxI     = Box<i32>;
using BoxL     = Box<i64>;
using BoxUC    = Box<u8>;
using BoxUS    = Box<u16>;
using BoxUI    = Box<u32>;
using BoxUL    = Box<u64>;
using BoxF     = Box<float>;
using BoxD     = Box<double>;
using BoxLD    = Box<long double>;
using BoxX     = Box<ARC_STD_FLOAT_TYPE>;
