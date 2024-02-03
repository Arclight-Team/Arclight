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
class Quaternion;


namespace CC {

	template<class T>
	concept Quaternion = SpecializationOf<T, Quaternion>;

}

namespace TT {

	template<CC::Quaternion V, CC::Quaternion... U>
	using CommonQuaternionType = Quaternion<CommonType<typename V::Type, typename U::Type...>>;

}


template<CC::Float T>
class Quaternion {

public:

	using Type = T;


	constexpr Quaternion() noexcept : x(0), y(0), z(0), w(0) {}

	constexpr Quaternion(T x, T y, T z, T w) noexcept : x(x), y(y), z(z), w(w) {}

	template<CC::Float F, CC::Float G>
	__ARC_CMATH26 Quaternion(const Vec3<F>& axis, G angle) { setAngleAxis(axis, angle); }

	__ARC_CMATH26 Quaternion(T yaw, T pitch, T roll) { setEulerAngles(yaw, pitch, roll); }


	constexpr T lengthSquared() const noexcept {
		return dot(*this);
	}

	__ARC_CMATH26 T length() const {
		return Math::sqrt(lengthSquared());
	}


	constexpr bool isReal() const noexcept {
		return Math::isZero(x) && Math::isZero(y) && Math::isZero(z);
	}

	constexpr bool isPure() const noexcept {
		return Math::isZero(w);
	}

	__ARC_CMATH26 bool isUnit() const {
		return Math::equal(length(), T(1));
	}


	__ARC_CMATH26 void normalize() {
		divide(length());
	}

	__ARC_CMATH26 Quaternion normalized() const {

		Quaternion ret = *this;
		ret.normalize();

		return ret;

	}

	constexpr void conjugate() noexcept {

		x = -x;
		y = -y;
		z = -z;

	}

	constexpr Quaternion conjugated() const noexcept {

		Quaternion ret = *this;
		ret.conjugate();

		return ret;

	}

	constexpr void invert() noexcept {

		conjugate();
		divide(lengthSquared());

	}

	constexpr Quaternion inverse() const {

		Quaternion ret = *this;
		ret.invert();

		return ret;

	}


	template<CC::Float F>
	constexpr Quaternion& add(const Quaternion<F>& q) noexcept {

		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;

		return *this;

	}

	template<CC::Float F>
	constexpr Quaternion& subtract(const Quaternion<F>& q) noexcept {

		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;

		return *this;

	}

	template<CC::Float F>
	constexpr Quaternion& multiply(const Quaternion<F>& q) noexcept {

		x = w * q.x + x * q.w + y * q.z - z * q.y;
		y = w * q.y + y * q.w + z * q.x - x * q.z;
		z = w * q.z + z * q.w + x * q.y - y * q.x;
		w = w * q.w - x * q.x - y * q.y - z * q.z;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Quaternion multiply(A scalar) noexcept {

		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Quaternion divide(A scalar) {

		arc_assert(!Math::isZero(scalar), "Cannot divide Quaternion by 0");

		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;

		return *this;

	}


	template<CC::Float F>
	constexpr T dot(const Quaternion<F>& q) const {
		return x * q.x + y * q.y + z * q.z + w * q.w;
	}


	template<CC::Float F>
	constexpr Quaternion& operator=(const Quaternion<F>& q) noexcept {

		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;

		return *this;

	}

	template<CC::Float F>
	constexpr Quaternion& operator+=(const Quaternion<F>& q) noexcept {
		return add(q);
	}

	template<CC::Float F>
	constexpr Quaternion& operator-=(const Quaternion<F>& q) noexcept {
		return subtract(q);
	}

	template<CC::Float F>
	constexpr Quaternion& operator*=(const Quaternion<F>& q) noexcept {
		return multiply(q);
	}

	template<CC::Arithmetic A>
	constexpr Quaternion& operator*=(A scalar) noexcept {
		return multiply(scalar);
	}

	template<CC::Arithmetic A>
	constexpr Quaternion& operator/=(A scalar) noexcept {
		return divide(scalar);
	}


	template<CC::Float F>
	constexpr bool operator==(const Quaternion<F>& q) const noexcept {
		return Math::equal(x, q.x) && Math::equal(y, q.y) && Math::equal(z, q.z) && Math::equal(w, q.w);
	}

	constexpr Quaternion operator-() const noexcept {
		return {-x, -y, -z, -w};
	}


	template<CC::Float F, CC::Float G>
	__ARC_CMATH26 void setAngleAxis(const Vec3<F>& axis, G angle) {

		T s = static_cast<T>(Math::sin(angle / T(2)));
		T c = static_cast<T>(Math::cos(angle / T(2)));

		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
		w = c;

	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	__ARC_CMATH26 void setEulerAngles(A yaw, B pitch, C roll) {

		const auto [ysin, ycos] = Math::sincos(yaw / T(2));
		const auto [psin, pcos] = Math::sincos(pitch / T(2));
		const auto [rsin, rcos] = Math::sincos(roll / T(2));

		x = ycos * pcos * rsin - ysin * psin * rcos;
		y = ycos * psin * rcos + ysin * pcos * rsin;
		z = ysin * pcos * rcos - ycos * psin * rsin;
		w = ycos * pcos * rcos + ysin * psin * rsin;

	}

	template<CC::Float F>
	__ARC_CMATH26 void setMat3(const Mat3<F>& m) {
		
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
	__ARC_CMATH26 void setMat4(const Mat4<F>& m) {
		setMat3(m.toMat3());
	}


	__ARC_CMATH26 Mat3<T> toMat3() const {

		Quaternion q = normalized();

		return {
			1 - 2 * (q.y * q.y + q.z * q.z), -2 * q.w * q.z + 2 * q.x * q.y, 2 * q.w * q.y + 2 * q.x * q.z,
			2 * q.w * q.z + 2 * q.x * q.y, 1 - 2 * (q.x * q.x + q.z * q.z), -2 * q.w * q.x + 2 * q.y * q.z,
			-2 * q.w * q.y + 2 * q.x * q.z, 2 * q.w * q.x + 2 * q.y * q.z, 1 - 2 * (q.x * q.x + q.y * q.y)
		};

	}

	__ARC_CMATH26 Mat4<T> toMat4() const {

		Quaternion q = normalized();

		return {
			1 - 2 * (q.y * q.y + q.z * q.z), -2 * q.w * q.z + 2 * q.x * q.y, 2 * q.w * q.y + 2 * q.x * q.z, 0,
			2 * q.w * q.z + 2 * q.x * q.y, 1 - 2 * (q.x * q.x + q.z * q.z), -2 * q.w * q.x + 2 * q.y * q.z, 0,
			-2 * q.w * q.y + 2 * q.x * q.z, 2 * q.w * q.x + 2 * q.y * q.z, 1 - 2 * (q.x * q.x + q.y * q.y), 0,
			0, 0, 0, 1
		};

	}


	template<CC::Float F, CC::Float G>
	constexpr static Quaternion fromAngleAxis(const Vec3<F>& axis, G angle) {
		Quaternion q;
		q.setAngleAxis(axis, angle);
		return q;
	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	constexpr static Quaternion fromEulerAngles(A yaw, B pitch, C roll) {
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


#ifndef ARC_QUATERNION_XYZW
	T w;
#endif
	T x;
	T y;
	T z;
#ifdef ARC_QUATERNION_XYZW
	T w;
#endif

};


template<CC::Quaternion Q, CC::Quaternion P>
constexpr TT::CommonQuaternionType<Q, P> operator+(Q q, const P& p) {

	TT::CommonQuaternionType<Q, P> r = q;
	return r += p;

}

template<CC::Quaternion Q, CC::Quaternion P>
constexpr TT::CommonQuaternionType<Q, P> operator-(Q q, const P& p) {

	TT::CommonQuaternionType<Q, P> r = q;
	return r -= p;

}


template<CC::Quaternion Q, CC::Quaternion P>
constexpr TT::CommonQuaternionType<Q, P> operator*(Q q, const P& p) {

	TT::CommonQuaternionType<Q, P> r = q;
	return r *= p;

}

template<CC::Float F, CC::Arithmetic A>
constexpr auto operator*(Quaternion<F> q, A scalar) {

	Quaternion<TT::CommonType<F, A>> p = q;
	return p *= scalar;

}


template<CC::Float F, CC::Arithmetic A>
constexpr auto operator/(Quaternion<F> q, A scalar) {

	Quaternion<TT::CommonType<F, A>> p = q;
	return p /= scalar;

}


template<CC::Float F>
RawLog& operator<<(RawLog& log, const Quaternion<F>& quat) {

	log << "Quat[";
	log << quat.w << ", ";
	log << quat.x << ", ";
	log << quat.y << ", ";
	log << quat.z << "]";

	return log;

}


ARC_DEFINE_FLOAT_ALIASES(1, Quaternion, Quaternion)
ARC_DEFINE_FLOAT_ALIASES(1, Quaternion, Quat)
