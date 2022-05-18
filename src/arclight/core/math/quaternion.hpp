/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 quaternion.hpp
 */

#pragma once

#include "math/math.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "arcconfig.hpp"



template<CC::Float T>
class Quaternion {

public:

	using Type = T;

	constexpr Quaternion() : x(T(0)), y(T(0)), z(T(0)), w(T(1)) {}
	constexpr explicit Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

	template<CC::Float F, CC::Float G>
	constexpr explicit Quaternion(const Vec3<F>& axis, G angle) {
		*this = Quaternion::fromAngleAxis(axis, angle);
	}

	constexpr explicit Quaternion(T yaw, T pitch, T roll) {
		*this = Quaternion::fromEulerAngles(yaw, pitch, roll);
	}


	constexpr Quaternion normalized() const {
		return normalize(*this);
	}

	constexpr void normalize() {
		divide(length());
	}

	constexpr T lengthSquared() const {
		return dot(*this);
	}

	constexpr T length() const {
		return Math::sqrt(lengthSquared());
	}


	template<CC::Float F>
	constexpr void add(const Quaternion<F>& q) {

		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;

	}

	template<CC::Float F>
	constexpr void subtract(const Quaternion<F>& q) {

		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;

	}

	template<CC::Float F>
	constexpr void multiply(const Quaternion<F>& q) {

		T tx = w * q.x + x * q.w + y * q.z - z * q.y;
		T ty = w * q.y + y * q.w + z * q.x - x * q.z;
		T tz = w * q.z + z * q.w + x * q.y - y * q.x;
		T tw = w * q.w - x * q.x - y * q.y - z * q.z;

		x = tx;
		y = ty;
		z = tz;
		w = tw;

	}

	template<CC::Arithmetic A>
	constexpr void multiply(A scalar) {

		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;

	}

	template<CC::Arithmetic A>
	constexpr void divide(A scalar) {

		arc_assert(!Math::isZero(scalar), "Cannot divide Quaternion by 0");
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;

	}

	constexpr bool isReal() const {
		return Math::isZero(x) && Math::isZero(y) && Math::isZero(z);
	}

	constexpr bool isPure() const {
		return Math::isZero(w);
	}

	constexpr bool isUnit() const {
		return Math::equal(length(), T(1));
	}

	constexpr void conjugate() {
		x = -x;
		y = -y;
		z = -z;
	}

	constexpr Quaternion conjugated() const {
		return Quaternion(-x, -y, -z, w);
	}

	constexpr void invert() {

		conjugate();
		divide(lengthSquared());

	}

	constexpr Quaternion inverse() const {

		Quaternion q(*this);
		q.invert();
		return q;

	}

	template<CC::Float F>
	constexpr T dot(const Quaternion<F>& q) const {
		return x * q.x + y * q.y + z * q.z + w * q.w;
	}


	template<CC::Float F>
	constexpr Quaternion& operator=(const Quaternion<F>& q) {

		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
		return *this;

	}

	template<CC::Float F>
	constexpr Quaternion& operator+=(const Quaternion<F>& q) {
		add(q);
		return *this;
	}

	template<CC::Float F>
	constexpr Quaternion& operator-=(const Quaternion<F>& q) {
		subtract(q);
		return *this;
	}

	template<CC::Float F>
	constexpr Quaternion& operator*=(const Quaternion<F>& q) {
		multiply(q);
		return *this;
	}

	template<CC::Arithmetic A>
	constexpr Quaternion& operator*=(A scalar) {
		multiply(scalar);
		return *this;
	}

	template<CC::Arithmetic A>
	constexpr Quaternion& operator/=(A scalar) {
		divide(scalar);
		return *this;
	}

	template<CC::Float F>
	constexpr bool operator==(const Quaternion<F>& q) const {
		return Math::equal(x, q.x) && Math::equal(y, q.y) && Math::equal(z, q.z) && Math::equal(w, q.w);
	}

	constexpr Quaternion operator-() const {
		return Quaternion(-x, -y, -z, -w);
	}


	template<CC::Float F, CC::Float G>
	constexpr void setAngleAxis(const Vec3<F>& axis, G angle) {

		T s = static_cast<T>(Math::sin(angle / static_cast<T>(2)));
		T c = static_cast<T>(Math::cos(angle / static_cast<T>(2)));

		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
		w = c;

	}

	constexpr void setEulerAngles(T yaw, T pitch, T roll) {

		T sa = Math::sin(yaw * static_cast<T>(0.5));
		T ca = Math::cos(yaw * static_cast<T>(0.5));
		T sb = Math::sin(pitch * static_cast<T>(0.5));
		T cb = Math::cos(pitch * static_cast<T>(0.5));
		T sc = Math::sin(roll * static_cast<T>(0.5));
		T cc = Math::cos(roll * static_cast<T>(0.5));

		x = ca * cb * sc - sa * sb * cc;
		y = ca * sb * cc + sa * cb * sc;
		z = sa * cb * cc - ca * sb * sc;
		w = ca * cb * cc + sa * sb * sc;

	}

	template<CC::Float F>
	constexpr void setMat3(const Mat3<F>& m) {
		
#ifdef ARC_QUATERNION_MATCONV_BRANCHLESS
		x = T(0.5) * Math::sqrt(1 + m[0][0] - m[1][1] - m[2][2]);
		y = T(0.5) * Math::sqrt(1 - m[0][0] + m[1][1] - m[2][2]);
		z = T(0.5) * Math::sqrt(1 - m[0][0] - m[1][1] + m[2][2]);
		w = T(0.5) * Math::sqrt(1 + m[0][0] + m[1][1] + m[2][2]);
		x = Math::copysign(x, m[2][1] - m[1][2]);
		y = Math::copysign(y, m[0][2] - m[2][0]);
		z = Math::copysign(z, m[1][0] - m[0][1]);
#else
		T t = m.trace();

		if(t >= 0) {

			T r = Math::sqrt(1 + t);
			T s = 1 / (2 * r);
			w = T(0.5) * r;
			x = s * (m[2][1] - m[1][2]);
			y = s * (m[0][2] - m[2][0]);
			z = s * (m[1][0] - m[0][1]);

		} else if (m[0][0] > m[1][1] && m[0][0] > m[2][2]) {

			T r = Math::sqrt(1 + m[0][0] - m[1][1] - m[2][2]);
			T s = 1 / (2 * r);
			w = s * (m[2][1] - m[1][2]);
			x = T(0.5) * r;
			y = s * (m[0][1] + m[1][0]);
			z = s * (m[2][0] + m[0][2]);

		} else if (m[1][1] > m[0][0] && m[1][1] > m[2][2]) {

			T r = Math::sqrt(1 - m[0][0] + m[1][1] - m[2][2]);
			T s = 1 / (2 * r);
			w = s * (m[0][2] - m[2][0]);
			x = s * (m[0][1] + m[1][0]);
			y = T(0.5) * r;
			z = s * (m[1][2] + m[2][1]);

		} else {

			T r = Math::sqrt(1 - m[0][0] - m[1][1] + m[2][2]);
			T s = 1 / (2 * r);
			w = s * (m[1][0] - m[0][1]);
			x = s * (m[0][2] + m[2][0]);
			y = s * (m[1][2] + m[2][1]);
			z = T(0.5) * r;

		}
#endif

	}

	template<CC::Float F>
	constexpr void setMat4(const Mat4<F>& m) {
		setMat3(m.toMat3());
	}

	constexpr Mat3<T> toMat3() const {

		Quaternion q = normalized();

		return Mat3<T>( 1 - 2 * (q.y * q.y + q.z * q.z), -2 * q.w * q.z + 2 * q.x * q.y, 2 * q.w * q.y + 2 * q.x * q.z,
						2 * q.w * q.z + 2 * q.x * q.y, 1 - 2 * (q.x * q.x + q.z * q.z), -2 * q.w * q.x + 2 * q.y * q.z,
						-2 * q.w * q.y + 2 * q.x * q.z, 2 * q.w * q.x + 2 * q.y * q.z, 1 - 2 * (q.x * q.x + q.y * q.y));

	}

	constexpr Mat4<T> toMat4() const {

		Quaternion q = normalized();

		return Mat4<T>( 1 - 2 * (q.y * q.y + q.z * q.z), -2 * q.w * q.z + 2 * q.x * q.y, 2 * q.w * q.y + 2 * q.x * q.z, 0,
						2 * q.w * q.z + 2 * q.x * q.y, 1 - 2 * (q.x * q.x + q.z * q.z), -2 * q.w * q.x + 2 * q.y * q.z, 0,
						-2 * q.w * q.y + 2 * q.x * q.z, 2 * q.w * q.x + 2 * q.y * q.z, 1 - 2 * (q.x * q.x + q.y * q.y), 0,
						0, 0, 0, 1);

	}

	template<CC::Float F, CC::Float G>
	constexpr static Quaternion fromAngleAxis(const Vec3<F>& axis, G angle) {
	   Quaternion q;
	   q.setAngleAxis(axis, angle);
	   return q;
	}

	constexpr static Quaternion fromEulerAngles(T yaw, T pitch, T roll) {
		Quaternion q;
		q.setEulerAngles(yaw, pitch, roll);
		return q;
	}

	template<CC::Float F>
	constexpr static Quaternion fromMat3(const Mat3<F>& m) {
		Quaternion q;
		q.setMat3(m);
		return q;
	}

	template<CC::Float F>
	constexpr static Quaternion fromMat4(const Mat4<F>& m) {
		Quaternion q;
		q.setMat4(m);
		return q;
	}

	constexpr static Quaternion normalize(Quaternion v) {
		v.normalize();
		return v;
	}

	constexpr static Quaternion conjugate(Quaternion v) {
		v.conjugate();
		return v;
	}

#ifdef ARC_QUATERNION_XYZW
	T x, y, z, w;
#else
	T w, x, y, z;
#endif

};



template<CC::Float F, CC::Float G>
constexpr auto operator+(Quaternion<F> q, const Quaternion<G>& p) {

	Quaternion<decltype(q.x + p.x)> r = q;
	r += p;
	return r;

}

template<CC::Float F, CC::Float G>
constexpr auto operator-(Quaternion<F> q, const Quaternion<G>& p) {

	Quaternion<decltype(q.x - p.x)> r = q;
	r -= p;
	return r;

}

template<CC::Float F, CC::Float G>
constexpr auto operator*(Quaternion<F> q, const Quaternion<G>& p) {

	Quaternion<decltype(q.x * p.x)> r = q;
	r *= p;
	return r;

}

template<CC::Float F, CC::Arithmetic A>
constexpr auto operator*(Quaternion<F> q, A scalar) {

	Quaternion<decltype(q.x * scalar)> p = q;
	p *= scalar;
	return p;

}

template<CC::Float F, CC::Arithmetic A>
constexpr auto operator/(Quaternion<F> q, A scalar) {

	Quaternion<decltype(q.x / scalar)> p = q;
	p /= scalar;
	return p;

}



#define QUATERNION_DEFINE_NTS(name, type, suffix) typedef Quaternion<type> name##suffix;

#define QUATERNION_DEFINE_N(name) \
	QUATERNION_DEFINE_NTS(name, float, F) \
	QUATERNION_DEFINE_NTS(name, double, D) \
	QUATERNION_DEFINE_NTS(name, long double, LD) \
	QUATERNION_DEFINE_NTS(name, ARC_STD_FLOAT_TYPE, X)

#define QUATERNION_DEFINE \
	QUATERNION_DEFINE_N(Quat) \
	QUATERNION_DEFINE_N(Quaternion)

QUATERNION_DEFINE

#undef QUATERNION_DEFINE
#undef QUATERNION_DEFINE_N
#undef QUATERNION_DEFINE_NTS
