#pragma once

#include "util/math.h"
#include "util/vector.h"


template<Float T>
class Mat2 {

public:

	constexpr Mat2() : v{ { 1, 0 }, { 0, 1 } } {}

	template<Arithmetic A>
	constexpr Mat2(const Mat2<A>& m) : v{ m[0], m[1] } {}


	template<Arithmetic A>
	constexpr void add(const Mat2<A>& m) {
		v[0] += m[0];
		v[1] += m[0];
	}

	template<Arithmetic A>
	constexpr void subtract(const Mat2<A>& m) {
		v[0] -= m[0];
		v[1] -= m[1];
	}

	template<Arithmetic A>
	constexpr void multiply(const Mat2<A>& m) {
		v[0][0] = v[0][0] * m[0][0] + v[1][0] * m[0][1];
		v[0][1] = v[0][1] * m[0][0] + v[1][1] * m[0][1];
		v[1][0] = v[0][0] * m[1][0] + v[1][0] * m[1][1];
		v[1][1] = v[0][1] * m[1][0] + v[1][1] * m[1][1];
	}

	template<Arithmetic A>
	constexpr void compMultiply(const Mat2<A>& m) {
		v[0][0] *= m[0][0];
		v[0][1] *= m[0][1];
		v[1][0] *= m[1][0];
		v[1][1] *= m[1][1];
	}

	constexpr void transpose() {
		T t = v[0][1];
		v[0][1] = v[1][0];
		v[1][0] = t;
	}

	constexpr T determinant() {
		return v[0][0] * v[1][1] - v[1][0] * v[0][1];
	}

	constexpr void invert() {

		T det = determinant();
		arc_assert(!Math::isZero(det), "Mat2 inverse failed: Matrix not invertible");

		T f = 1.0 / det;

		T ta = v[0][0] * f;
		v[0][0] = v[1][1] * f;
		v[0][1] = -v[0][1] * f;
		v[1][0] = -v[1][0] * f;
		v[1][1] = ta;

	}

	template<Arithmetic A>
	constexpr Mat2& operator=(const Mat2<A>& m) {
		v[0] = m[0];
		v[1] = m[1];
		return *this;
	}

	constexpr Vec2<T>& operator[](u32 index) {
		arc_assert(index < 2, "Mat2 access out of bounds (index=%d)", index);
		return v[index];
	}


	//constexpr static Mat2<T> identity(Vec2(1, 0), Vec2(0, 1));


	Vec2<T> v[2];

};