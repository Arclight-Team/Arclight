#pragma once

#include "util/math.h"
#include "util/vector.h"



template<Float T>
class Mat2 {

public:

	constexpr Mat2() {
		setIdentity();
	}

	template<Float A>
	constexpr Mat2(const Mat2<A>& m) : v{ m[0], m[1] } {}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D>
	constexpr Mat2(A a, B b, C c, D d) : v{ {a, c}, {b, d} } {}

	template<Arithmetic A, Arithmetic B>
	constexpr Mat2(const Vec2<A>& a, const Vec2<B>& b) : v{ a, b } {}


	template<Float A>
	constexpr Mat2& add(const Mat2<A>& m) {
		v[0] += m[0];
		v[1] += m[1];
		return *this;
	}

	template<Float A>
	constexpr Mat2& subtract(const Mat2<A>& m) {
		v[0] -= m[0];
		v[1] -= m[1];
		return *this;
	}

	template<Float A>
	constexpr Mat2& multiply(const Mat2<A>& m) {
		T a = v[0][0] * m[0][0] + v[1][0] * m[0][1];
		T b = v[0][1] * m[0][0] + v[1][1] * m[0][1];
		T c = v[0][0] * m[1][0] + v[1][0] * m[1][1];
		T d = v[0][1] * m[1][0] + v[1][1] * m[1][1];
		*this = Mat2(a, c, b, d);
		return *this;
	}

	template<Arithmetic A>
	constexpr Mat2& multiply(A a) {
		v[0] *= a;
		v[1] *= a;
		return *this;
	}

	template<Float A>
	constexpr Mat2& compMultiply(const Mat2<A>& m) {
		v[0].compMultiply(m[0]);
		v[1].compMultiply(m[1]);
		return *this;
	}

	constexpr void transpose() {
		T t = v[0][1];
		v[0][1] = v[1][0];
		v[1][0] = t;
	}

	constexpr Mat2 transpose() const {
		return Mat2(v[0][0], v[0][1], v[1][0], v[1][1]);
	}

	constexpr T determinant() {
		return v[0][0] * v[1][1] - v[1][0] * v[0][1];
	}

	constexpr void invert() {

		T det = determinant();
		arc_assert(!Math::isZero(det), "Mat2 inverse failed: Matrix not invertible");

		T s = 1.0 / det;

		T t = v[0][0] * s;
		v[0][0] = v[1][1] * s;
		v[0][1] = -v[0][1] * s;
		v[1][0] = -v[1][0] * s;
		v[1][1] = t;

	}

	constexpr Mat2 inverse() const {

		Mat2 ret = *this;
		ret.invert();

		return ret;

	}

	constexpr void setIdentity() {
		v[0] = {1, 0};
		v[1] = {0, 1};
	}

	template<Float A>
	constexpr Mat2& operator=(const Mat2<A>& m) {
		v[0] = m[0];
		v[1] = m[1];
		return *this;
	}

	template<Float A>
	constexpr Mat2& operator+=(const Mat2<A>& m) {
		return add(m);
	}

	template<Float A>
	constexpr Mat2& operator-=(const Mat2<A>& m) {
		return subtract(m);
	}

	template<Float A>
	constexpr Mat2& operator*=(const Mat2<A>& m) {
		return multiply(m);
	}

	template<Arithmetic A>
	constexpr Mat2& operator*=(A a) {
		return multiply(a);
	}

	template<Float A>
	constexpr bool operator==(const Mat2<A>& m) const {
		return v[0] == m[0] && v[1] == m[1];
	}

	template<Float A>
	constexpr bool operator!=(const Mat2<A>& m) const {
		return !(*this == m);
	}

	constexpr Vec2<T>& operator[](u32 index) {
		arc_assert(index < 2, "Mat2 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr const Vec2<T>& operator[](u32 index) const {
		arc_assert(index < 2, "Mat2 access out of bounds (index=%d)", index);
		return v[index];
	}


	Vec2<T> v[2];

};



template<Float T>
class Mat3 {

public:

	constexpr Mat3() {
		setIdentity();
	}

	template<Float A>
	constexpr Mat3(const Mat3<A>& m) : v{ m[0], m[1], m[2] } {}

	template<Arithmetic A, Arithmetic B, Arithmetic C, 
			 Arithmetic D, Arithmetic E, Arithmetic F,
			 Arithmetic G, Arithmetic H, Arithmetic I>
	constexpr Mat3(A a, B b, C c, D d, E e, F f, G g, H h, I i) : v{ {a, d, g}, {b, e, h}, {c, f, i} } {}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr Mat3(const Vec3<A>& a, const Vec3<B>& b, const Vec3<C>& c) : v{ a, b, c } {}


	template<Float A>
	constexpr Mat3& add(const Mat3<A>& m) {
		v[0] += m[0];
		v[1] += m[1];
		v[2] += m[2];
		return *this;
	}

	template<Float A>
	constexpr Mat3& subtract(const Mat3<A>& m) {
		v[0] -= m[0];
		v[1] -= m[1];
		v[2] -= m[2];
		return *this;
	}

	template<Float A>
	constexpr Mat3& multiply(const Mat3<A>& m) {
		T a = v[0][0] * m[0][0] + v[1][0] * m[0][1] + v[2][0] * m[0][2];
		T b = v[0][1] * m[0][0] + v[1][1] * m[0][1] + v[2][1] * m[0][2];
		T c = v[0][2] * m[0][0] + v[1][2] * m[0][1] + v[2][2] * m[0][2];
		T d = v[0][0] * m[1][0] + v[1][0] * m[1][1] + v[2][0] * m[1][2];
		T e = v[0][1] * m[1][0] + v[1][1] * m[1][1] + v[2][1] * m[1][2];
		T f = v[0][2] * m[1][0] + v[1][2] * m[1][1] + v[2][2] * m[1][2];
		T g = v[0][0] * m[2][0] + v[1][0] * m[2][1] + v[2][0] * m[2][2];
		T h = v[0][1] * m[2][0] + v[1][1] * m[2][1] + v[2][1] * m[2][2];
		T i = v[0][2] * m[2][0] + v[1][2] * m[2][1] + v[2][2] * m[2][2];
		*this = Mat3(a, d, h, b, e, h, c, f, i);
		return *this;
	}
	
	template<Arithmetic A>
	constexpr Mat3& multiply(A a) {
		v[0] *= a;
		v[1] *= a;
		v[2] *= a;
		return *this;
	}

	template<Float A>
	constexpr Mat3& compMultiply(const Mat3<A>& m) {
		v[0].compMultiply(m[0]);
		v[1].compMultiply(m[1]);
		v[2].compMultiply(m[2]);
		return *this;
	}

	constexpr void transpose() {

		T t = v[0][1];
		v[0][1] = v[1][0];
		v[1][0] = t;

		t = v[0][2];
		v[0][2] = v[2][0];
		v[2][0] = t;

		t = v[1][2];
		v[1][2] = v[2][1];
		v[2][1] = t;

	}

	constexpr Mat3 transpose() const {
		return Mat3(v[0][0], v[0][1], v[0][2], v[1][0], v[1][1], v[1][2], v[2][0], v[2][1], v[2][2]);
	}

	constexpr T determinant() {
		return v[0][0] * v[1][1] * v[2][2] + v[1][0] * v[2][1] * v[0][2] + v[2][0] * v[0][1] * v[1][2]
			 - v[0][2] * v[1][1] * v[2][0] - v[1][2] * v[2][1] * v[0][0] - v[2][2] * v[0][1] * v[1][0];
	}

	constexpr void invert() {

		T det = determinant();
		arc_assert(!Math::isZero(det), "Mat3 inverse failed: Matrix not invertible");

		T s = 1.0 / det;

		T a = v[0][0];
		T b = v[1][0];
		T c = v[2][0];
		T d = v[0][1];
		T e = v[1][1];
		T f = v[2][1];
		T g = v[0][2];
		T h = v[1][2];
		T i = v[2][2];

		v[0][0] = (e * i - f * h) * s;
		v[0][1] = (f * g - d * i) * s;
		v[0][2] = (d * h - e * g) * s;
		v[1][0] = (c * h - b * i) * s;
		v[1][1] = (a * i - c * g) * s;
		v[1][2] = (b * g - a * h) * s;
		v[2][0] = (b * f - c * e) * s;
		v[2][1] = (c * d - a * f) * s;
		v[2][2] = (a * e - b * d) * s;

	}

	constexpr Mat3 inverse() const {

		Mat3 ret = *this;
		ret.invert();

		return ret;

	}

	constexpr void setIdentity() {
		v[0] = { 1, 0, 0 };
		v[1] = { 0, 1, 0 };
		v[2] = { 0, 0, 1 };
	}

	template<Float A>
	constexpr Mat3& operator=(const Mat3<A>& m) {
		v[0] = m[0];
		v[1] = m[1];
		v[2] = m[2];
		return *this;
	}

	template<Float A>
	constexpr Mat3& operator+=(const Mat3<A>& m) {
		return add(m);
	}

	template<Float A>
	constexpr Mat3& operator-=(const Mat3<A>& m) {
		return subtract(m);
	}

	template<Float A>
	constexpr Mat3& operator*=(const Mat3<A>& m) {
		return multiply(m);
	}

	template<Arithmetic A>
	constexpr Mat3& operator*=(A a) {
		return multiply(a);
	}

	template<Float A>
	constexpr bool operator==(const Mat3<A>& m) const {
		return v[0] == m[0] && v[1] == m[1] && v[2] == m[2];
	}

	template<Float A>
	constexpr bool operator!=(const Mat3<A>& m) const {
		return !(*this == m);
	}

	constexpr Vec3<T>& operator[](u32 index) {
		arc_assert(index < 3, "Mat3 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr const Vec3<T>& operator[](u32 index) const {
		arc_assert(index < 3, "Mat3 access out of bounds (index=%d)", index);
		return v[index];
	}


	Vec3<T> v[3];

};



template<Float T>
class Mat4 {

public:

	constexpr Mat4() {
		setIdentity();
	}

	template<Float A>
	constexpr Mat4(const Mat4<A>& m) : v{ m[0], m[1], m[2], m[3] } {}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D,
			 Arithmetic E, Arithmetic F, Arithmetic G, Arithmetic H,
			 Arithmetic I, Arithmetic J, Arithmetic K, Arithmetic L,
			 Arithmetic M, Arithmetic N, Arithmetic O, Arithmetic P>
	constexpr Mat4(A a, B b, C c, D d, E e, F f, G g, H h,
				   I i, J j, K k, L l, M m, N n, O o, P p) : v{ {a, e, i, m}, {b, f, j, n}, {c, g, k, o}, {d, h, l, p} } {}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D>
	constexpr Mat4(const Vec4<A>& a, const Vec4<B>& b, const Vec4<C>& c, const Vec4<D>& d) : v{ a, b, c, d } {}


	template<Float A>
	constexpr Mat4& add(const Mat4<A>& m) {
		v[0] += m[0];
		v[1] += m[1];
		v[2] += m[2];
		v[3] += m[3];
		return *this;
	}

	template<Float A>
	constexpr Mat4& subtract(const Mat4<A>& m) {
		v[0] -= m[0];
		v[1] -= m[1];
		v[2] -= m[2];
		v[3] -= m[3];
		return *this;
	}

	template<Float A>
	constexpr Mat4& multiply(const Mat4<A>& t) {
		T a = v[0][0] * t[0][0] + v[1][0] * t[0][1] + v[2][0] * t[0][2] + v[3][0] * t[0][3];
		T b = v[0][1] * t[0][0] + v[1][1] * t[0][1] + v[2][1] * t[0][2] + v[3][1] * t[0][3];
		T c = v[0][2] * t[0][0] + v[1][2] * t[0][1] + v[2][2] * t[0][2] + v[3][2] * t[0][3];
		T d = v[0][3] * t[0][0] + v[1][3] * t[0][1] + v[2][3] * t[0][2] + v[3][3] * t[0][3];
		T e = v[0][0] * t[1][0] + v[1][0] * t[1][1] + v[2][0] * t[1][2] + v[3][0] * t[1][3];
		T f = v[0][1] * t[1][0] + v[1][1] * t[1][1] + v[2][1] * t[1][2] + v[3][1] * t[1][3];
		T g = v[0][2] * t[1][0] + v[1][2] * t[1][1] + v[2][2] * t[1][2] + v[3][2] * t[1][3];
		T h = v[0][3] * t[1][0] + v[1][3] * t[1][1] + v[2][3] * t[1][2] + v[3][3] * t[1][3];
		T i = v[0][0] * t[2][0] + v[1][0] * t[2][1] + v[2][0] * t[2][2] + v[3][0] * t[2][3];
		T j = v[0][1] * t[2][0] + v[1][1] * t[2][1] + v[2][1] * t[2][2] + v[3][1] * t[2][3];
		T k = v[0][2] * t[2][0] + v[1][2] * t[2][1] + v[2][2] * t[2][2] + v[3][2] * t[2][3];
		T l = v[0][3] * t[2][0] + v[1][3] * t[2][1] + v[2][3] * t[2][2] + v[3][3] * t[2][3];
		T m = v[0][0] * t[3][0] + v[1][0] * t[3][1] + v[2][0] * t[3][2] + v[3][0] * t[3][3];
		T n = v[0][1] * t[3][0] + v[1][1] * t[3][1] + v[2][1] * t[3][2] + v[3][1] * t[3][3];
		T o = v[0][2] * t[3][0] + v[1][2] * t[3][1] + v[2][2] * t[3][2] + v[3][2] * t[3][3];
		T p = v[0][3] * t[3][0] + v[1][3] * t[3][1] + v[2][3] * t[3][2] + v[3][3] * t[3][3];
		*this = Mat4(a, e, i, m, b, f, j, n, c, g, k, o, d, h, l, p);
		return *this;
	}

	template<Arithmetic A>
	constexpr Mat4& multiply(A a) {
		v[0] *= a;
		v[1] *= a;
		v[2] *= a;
		v[3] *= a;
		return *this;
	}

	template<Float A>
	constexpr Mat4& compMultiply(const Mat4<A>& m) {
		v[0].compMultiply(m[0]);
		v[1].compMultiply(m[1]);
		v[2].compMultiply(m[2]);
		v[3].compMultiply(m[3]);
		return *this;
	}

	constexpr void transpose() {

		T t = v[0][1];
		v[0][1] = v[1][0];
		v[1][0] = t;

		t = v[0][2];
		v[0][2] = v[2][0];
		v[2][0] = t;

		t = v[1][2];
		v[1][2] = v[2][1];
		v[2][1] = t;

		t = v[0][3];
		v[0][3] = v[3][0];
		v[3][0] = t;

		t = v[1][3];
		v[1][3] = v[3][1];
		v[3][1] = t;

		t = v[2][3];
		v[2][3] = v[3][2];
		v[3][2] = t;

	}

	constexpr Mat4 transpose() const {
		return Mat4(v[0][0], v[0][1], v[0][2], v[0][3], v[1][0], v[1][1], v[1][2], v[1][3],
					v[2][0], v[2][1], v[2][2], v[2][3], v[3][0], v[3][1], v[3][2], v[3][3]);
	}

	constexpr T determinant() {
		
		T a = v[0][0] * (v[1][1] * v[2][2] * v[3][3] + v[2][1] * v[3][2] * v[1][3] + v[3][1] * v[1][2] * v[2][3]
						-v[1][3] * v[2][2] * v[3][1] - v[2][3] * v[3][2] * v[1][1] - v[3][3] * v[1][2] * v[2][1]);
		T b = v[1][0] * (v[0][1] * v[2][2] * v[3][3] + v[2][1] * v[3][2] * v[0][3] + v[3][1] * v[0][2] * v[2][3]
						-v[0][3] * v[2][2] * v[3][1] - v[2][3] * v[3][2] * v[0][1] - v[3][3] * v[0][2] * v[2][1]);
		T c = v[2][0] * (v[0][1] * v[1][2] * v[3][3] + v[1][1] * v[3][2] * v[0][3] + v[3][1] * v[0][2] * v[1][3]
						-v[0][3] * v[1][2] * v[3][1] - v[1][3] * v[3][2] * v[0][1] - v[3][3] * v[0][2] * v[1][1]);
		T d = v[3][0] * (v[0][1] * v[1][2] * v[2][3] + v[1][1] * v[2][2] * v[0][3] + v[2][1] * v[0][2] * v[1][3]
						-v[0][3] * v[1][2] * v[2][1] - v[1][3] * v[2][2] * v[0][1] - v[2][3] * v[0][2] * v[1][1]);

		return a - b + c - d;
	
	}

	constexpr void invert() {

		T det = determinant();
		arc_assert(!Math::isZero(det), "Mat4 inverse failed: Matrix not invertible");

		T s = 1.0 / det;

		T a = v[0][0];
		T b = v[1][0];
		T c = v[2][0];
		T d = v[3][0];
		T e = v[0][1];
		T f = v[1][1];
		T g = v[2][1];
		T h = v[3][1];
		T i = v[0][2];
		T j = v[1][2];
		T k = v[2][2];
		T l = v[3][2];
		T m = v[0][3];
		T n = v[1][3];
		T o = v[2][3];
		T p = v[3][3];

		v[0][0] = ( f * k * p - f * l * o - j * g * p + j * h * o + n * g * l - n * h * k) * s;
		v[0][1] = (-e * k * p + e * l * o + i * g * p - i * h * o - m * g * l + m * h * k) * s;
		v[0][2] = ( e * j * p - e * l * n - i * f * p + i * h * n + m * f * l - m * h * j) * s;
		v[0][3] = (-e * j * o + e * k * n + i * f * o - i * g * n - m * f * k + m * g * j) * s;
		v[1][0] = (-b * k * p + b * l * o + j * c * p - j * d * o - n * c * l + n * d * k) * s;
		v[1][1] = ( a * k * p - a * l * o - i * c * p + i * d * o + m * c * l - m * d * k) * s;
		v[1][2] = (-a * j * p + a * l * n + i * b * p - i * d * n - m * b * l + m * d * j) * s;
		v[1][3] = ( a * j * o - a * k * n - i * b * o + i * c * n + m * b * k - m * c * j) * s;
		v[2][0] = ( b * g * p - b * h * o - f * c * p + f * d * o + n * c * h - n * d * g) * s;
		v[2][1] = (-a * g * p + a * h * o + e * c * p - e * d * o - m * c * h + m * d * g) * s;
		v[2][2] = ( a * f * p - a * h * n - e * b * p + e * d * n + m * b * h - m * d * f) * s;
		v[2][3] = (-a * f * o + a * g * n + e * b * o - e * c * n - m * b * g + m * c * f) * s;
		v[3][0] = (-b * g * l + b * h * k + f * c * l - f * d * k - j * c * h + j * d * g) * s;
		v[3][1] = ( a * g * l - a * h * k - e * c * l + e * d * k + i * c * h - i * d * g) * s;
		v[3][2] = (-a * f * l + a * h * j + e * b * l - e * d * j - i * b * h + i * d * f) * s;
		v[3][3] = ( a * f * k - a * g * j - e * b * k + e * c * j + i * b * g - i * c * f) * s;

	}

	constexpr Mat4 inverse() const {

		Mat4 ret = *this;
		ret.invert();

		return ret;
	
	}

	constexpr void setIdentity() {
		v[0] = { 1, 0, 0, 0 };
		v[1] = { 0, 1, 0, 0 };
		v[2] = { 0, 0, 1, 0 };
		v[3] = { 0, 0, 0, 1 };
	}

	template<Float A>
	constexpr Mat4& operator=(const Mat4<A>& m) {
		v[0] = m[0];
		v[1] = m[1];
		v[2] = m[2];
		v[3] = m[3];
		return *this;
	}

	template<Float A>
	constexpr Mat4& operator+=(const Mat4<A>& m) {
		return add(m);
	}

	template<Float A>
	constexpr Mat4& operator-=(const Mat4<A>& m) {
		return subtract(m);
	}

	template<Float A>
	constexpr Mat4& operator*=(const Mat4<A>& m) {
		return multiply(m);
	}

	template<Arithmetic A>
	constexpr Mat4& operator*=(A a) {
		return multiply(a);
	}

	template<Float A>
	constexpr bool operator==(const Mat4<A>& m) const {
		return v[0] == m[0] && v[1] == m[1] && v[2] == m[2] && v[3] == m[3];
	}

	template<Float A>
	constexpr bool operator!=(const Mat4<A>& m) const {
		return !(*this == m);
	}

	constexpr Vec4<T>& operator[](u32 index) {
		arc_assert(index < 4, "Mat4 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr const Vec4<T>& operator[](u32 index) const {
		arc_assert(index < 4, "Mat4 access out of bounds (index=%d)", index);
		return v[index];
	}


	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr Mat4& translate(A x, B y, C z) {
		v[3] = v[0] * x + v[1] * y + v[2] * z + v[3];
		return *this;
	}

	template<Arithmetic A, Arithmetic B>
	constexpr Mat4& rotate(const Vec3<A>& axis, B angle) {

		Vec3<T> u = Vec3<T>::normalize(axis);
		auto sinTheta = Math::sin(angle);
		auto cosTheta = Math::cos(angle);
		auto mcosTheta = 1.0 - cosTheta;

		Mat3<T> n (u.x * u.x * mcosTheta + cosTheta, u.x * u.y * mcosTheta - u.z * sinTheta, u.x * u.z * mcosTheta + u.y * sinTheta,
				u.x * u.y * mcosTheta + u.z * sinTheta, u.y * u.y * mcosTheta + cosTheta, u.y * u.z * mcosTheta - u.x * sinTheta,
				u.x * u.z * mcosTheta - u.y * sinTheta, u.y * u.z * mcosTheta + u.x * sinTheta, u.z * u.z * mcosTheta + cosTheta);

		Vec4 x = v[0] * n[0][0] + v[1] * n[0][1] + v[2] * n[0][2];
		Vec4 y = v[0] * n[1][0] + v[1] * n[1][1] + v[2] * n[1][2];
		Vec4 z = v[0] * n[2][0] + v[1] * n[2][1] + v[2] * n[2][2];
		v[0] = x;
		v[1] = y;
		v[2] = z;

		return *this;

	}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr Mat4& scale(A x, B y, C z) {
		v[0] *= x;
		v[1] *= y;
		v[2] *= z;
		return *this;
	}


	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr static Mat4 fromTranslation(A x, B y, C z) {
		return Mat4(1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1);
	}

	template<Float A, Arithmetic B>
	constexpr static Mat4 fromRotation(const Vec3<A>& axis, B angle) {

		Vec3<T> u = Vec3<T>::normalize(axis);
		auto sinTheta = Math::sin(angle);
		auto cosTheta = Math::cos(angle);
		auto mcosTheta = 1.0 - cosTheta;

		return Mat4(u.x * u.x * mcosTheta + cosTheta,		u.x * u.y * mcosTheta - u.z * sinTheta, u.x * u.z * mcosTheta + u.y * sinTheta, 0,
					u.x * u.y * mcosTheta + u.z * sinTheta, u.y * u.y * mcosTheta + cosTheta,		u.y * u.z * mcosTheta - u.x * sinTheta, 0,
					u.x * u.z * mcosTheta - u.y * sinTheta, u.y * u.z * mcosTheta + u.x * sinTheta, u.z * u.z * mcosTheta + cosTheta, 0,
					0, 0, 0, 1);

	}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr static Mat4 fromScale(A x, B y, C z) {
		return Mat4(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
	}

	template<Float A, Float B>
	constexpr static Mat4 lookAt(const Vec3<A>& pos, const Vec3<B>& target, const Vec3<double>& up = Vec3<double>(0, 1, 0)) {

		Vec3<T> f = Vec3<T>::normalize(pos - target);
		Vec3<T> r = Vec3<T>::normalize(up.cross(f));
		Vec3<T> u = f.cross(r);

		return Mat4(r.x, r.y, r.z, -r.dot(pos),
					u.x, u.y, u.z, -u.dot(pos),
					f.x, f.y, f.z, -f.dot(pos),
					0, 0, 0, 1);

	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D, Arithmetic E, Arithmetic F>
	constexpr static Mat4 frustum(A left, B right, C bottom, D top, E near, F far) {

		T l = left;
		T r = right;
		T b = bottom;
		T t = top;
		T n = near;
		T f = far;

		return Mat4(2 * n / (r - l), 0, (r + l) / (r - l), 0,
					0, 2 * n / (t - b), (t + b) / (t - b), 0,
					0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
					0, 0, -1, 0);

	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D>
	constexpr static Mat4 perspective(A fovy, B aspect, C near, D far) {

		T t = Math::tan(fovy / 2.0);
		T a = aspect;
		T n = near;
		T f = far;

		return Mat4(1 / (a * t), 0, 0, 0,
					0, 1 / t, 0, 0,
					0, 0, -(f + n) / (f - n), -2 * f * n / (f - n),
					0, 0, -1, 0);

	}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D, Arithmetic E, Arithmetic F>
	constexpr static Mat4 ortho(A left, B right, C bottom, D top, E near, F far) {

		T l = left;
		T r = right;
		T b = bottom;
		T t = top;
		T n = near;
		T f = far;

		return Mat4(2 / (r - l), 0, 0, -(r + l) / (r - l),
					0, 2 / (t - b), 0, -(t + b) / (t - b),
					0, 0, -2 * (f - n), -(f + n) / (f - n),
					0, 0, 0, 1);

	}


	Vec4<T> v[4];

};



template<Float A, Float B, template<Float> class Matrix>
constexpr auto operator+(Matrix<A> m, const Matrix<B>& n) requires (std::is_same_v<Matrix<A>, Mat2<A>> || std::is_same_v<Matrix<A>, Mat3<A>> || std::is_same_v<Matrix<A>, Mat4<A>>) {
	m += n;
	return m;
}

template<Float A, Float B, template<Float> class Matrix>
constexpr auto operator-(Matrix<A> m, const Matrix<B>& n) requires (std::is_same_v<Matrix<A>, Mat2<A>> || std::is_same_v<Matrix<A>, Mat3<A>> || std::is_same_v<Matrix<A>, Mat4<A>>) {
	m -= n;
	return m;
}

template<Float A, Float B, template<Float> class Matrix>
constexpr auto operator*(Matrix<A> m, const Matrix<B>& n) requires (std::is_same_v<Matrix<A>, Mat2<A>> || std::is_same_v<Matrix<A>, Mat3<A>> || std::is_same_v<Matrix<A>, Mat4<A>>) {
	m *= n;
	return m;
}


template<Float A, Arithmetic B>
constexpr auto operator*(const Mat2<A>& m, const Vec2<B>& v) {

	auto x = m[0][0] * v[0] + m[1][0] * v[1];
	auto y = m[0][1] * v[0] + m[1][1] * v[1];

	return Vec2<A>(x, y);
	
}

template<Float A, Arithmetic B>
constexpr auto operator*(const Mat3<A>& m, const Vec3<B>& v) {

	auto x = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2];
	auto y = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2];
	auto z = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2];

	return Vec3<A>(x, y, z);

}

template<Float A, Arithmetic B>
constexpr auto operator*(const Mat4<A>& m, const Vec4<B>& v) {

	auto x = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	auto y = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	auto z = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	auto w = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];

	return Vec4<A>(x, y, z, w);

}



#define MATRIX_DEFINE_NDTS(name, dim, type, suffix) typedef Mat##dim<type> name##dim##suffix;

#define MATRIX_DEFINE_ND(name, dim) \
	MATRIX_DEFINE_NDTS(name, dim, float, f) \
	MATRIX_DEFINE_NDTS(name, dim, double, d) \
	MATRIX_DEFINE_NDTS(name, dim, long double, ld) \

#define MATRIX_DEFINE_N(name) \
	MATRIX_DEFINE_ND(name, 2) \
	MATRIX_DEFINE_ND(name, 3) \
	MATRIX_DEFINE_ND(name, 4)

#define MATRIX_DEFINE \
	MATRIX_DEFINE_N(Mat) \
	MATRIX_DEFINE_N(Matrix)

MATRIX_DEFINE

#undef MATRIX_DEFINE
#undef MATRIX_DEFINE_N
#undef MATRIX_DEFINE_ND
#undef MATRIX_DEFINE_NDTS