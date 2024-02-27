/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 vector.hpp
 */

#pragma once

#include "math.hpp"
#include "util/assert.hpp"
#include "common/typetraits.hpp"



template<CC::Arithmetic T>
class Vec2;

template<CC::Arithmetic T>
class Vec3;

template<CC::Arithmetic T>
class Vec4;


namespace CC {

	template<class T>
	concept Vector2 = SpecializationOf<T, Vec2>;

	template<class T>
	concept Vector3 = SpecializationOf<T, Vec3>;

	template<class T>
	concept Vector4 = SpecializationOf<T, Vec4>;

	template<class T>
	concept Vector = Vector2<T> || Vector3<T> || Vector4<T>;

	template<class T>
	concept FloatVector = Vector<T> && Float<typename T::Type>;

	template<class T>
	concept IntegerVector = Vector<T> && Integer<typename T::Type>;

	template<class T>
	concept IntegralVector = Vector<T> && Integral<typename T::Type>;

	template<class T>
	concept ArithmeticParam = Vector<T> || Arithmetic<T>;

	template<class T>
	concept FloatParam = Float<T> || FloatVector<T>;

	template<class T>
	concept IntegerParam = Integer<T> || IntegerVector<T>;

	template<class T>
	concept IntegralParam = Integral<T> || IntegralVector<T>;

	template<class T, class... U>
	concept EqualVectorRank = Vector<T> && (Vector<U> && ...) && ((T::Size == U::Size) && ...);

}

namespace TT {

	namespace Detail {

		template<CC::Vector V, CC::Arithmetic T>
		struct SizedVector {
			using Type = Conditional<V::Size == 2, Vec2<T>, Conditional<V::Size == 3, Vec3<T>, Vec4<T>>>;
		};

	}

	template<CC::Vector V, CC::Arithmetic T>
	using SizedVector = typename Detail::SizedVector<V, T>::Type;

	template<CC::Vector V, CC::EqualVectorRank<V>... U>
	using CommonVectorType = SizedVector<V, CommonType<typename V::Type, typename U::Type...>>;

}


template<CC::Arithmetic T>
class Vec2 {

private:

	using X = TT::PromotedType<T>;

public:

	using Type = T;

	constexpr static SizeT Size = 2;



	constexpr Vec2() noexcept : x(0), y(0) {}

	constexpr explicit Vec2(T value) noexcept : x(value), y(value) {}

	template<CC::Arithmetic A, CC::Arithmetic B>
	constexpr Vec2(A x, B y) noexcept : x(x), y(y) {}

	template<CC::Arithmetic A>
	constexpr Vec2(const Vec2<A>& v) noexcept : x(v.x), y(v.y) {}


	template<CC::Arithmetic A>
	constexpr Vec2& add(const Vec2<A>& v) noexcept {

		x += v.x;
		y += v.y;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec2& subtract(const Vec2<A>& v) noexcept {

		x -= v.x;
		y -= v.y;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec2& multiply(A s) noexcept {

		x *= s;
		y *= s;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec2& divide(A s) noexcept {

		arc_assert(!Math::isZero(s), "Vec2 divided by 0");

		x /= s;
		y /= s;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec2& compMultiply(const Vec2<A>& v) noexcept {

		x *= v.x;
		y *= v.y;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec2& compDivide(const Vec2<A>& v) noexcept {

		arc_assert(!v.isNull(), "Vec2 divided by 0");

		x /= v.x;
		y /= v.y;

		return *this;

	}


	template<CC::Arithmetic A>
	constexpr Vec2& operator=(const Vec2<A>& v) noexcept {

		x = v.x;
		y = v.y;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec2& operator+=(const Vec2<A>& v) noexcept {
		return add(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec2& operator-=(const Vec2<A>& v) noexcept {
		return subtract(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec2& operator*=(const Vec2<A>& v) noexcept {
		return compMultiply(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec2& operator/=(const Vec2<A>& v) noexcept {
		return compDivide(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec2& operator*=(A s) noexcept {
		return multiply(s);
	}

	template<CC::Arithmetic A>
	constexpr Vec2& operator/=(A s) noexcept {
		return divide(s);
	}


	template<CC::Arithmetic A>
	constexpr bool operator==(const Vec2<A>& v) const noexcept {
		return Math::equal(x, v.x) && Math::equal(y, v.y);
	}

	constexpr Vec2 operator-() const noexcept {
		return Vec2(-x, -y);
	}


	constexpr T& operator[](SizeT index) noexcept {

		switch (index) {
			case 0: return x;
			case 1: return y;
			default:
				arc_force_assert("Vec2 access out of bounds (index=%d)", index);
				std::unreachable();
		}

	}

	constexpr const T& operator[](SizeT index) const noexcept {

		switch (index) {
			case 0: return x;
			case 1: return y;
			default:
				arc_force_assert("Vec2 access out of bounds (index=%d)", index);
				std::unreachable();
		}

	}


	constexpr X magSquared() const noexcept {
		return x * x + y * y;
	}

	ARC_CONSTEXPR_CMATH26 X length() const {
		return Math::sqrt(magSquared());
	}

	ARC_CONSTEXPR_CMATH26 void normalize() {
		divide(length());
	}

	ARC_CONSTEXPR_CMATH26 Vec2 normalized() const {
		return normalize(*this);
	}

	constexpr bool isNull() const noexcept {
		return Math::isZero(x) && Math::isZero(y);
	}

	constexpr bool anyNegative() const noexcept {
		return Math::less(x, 0) || Math::less(y, 0);
	}


	template<CC::Arithmetic A>
	constexpr X dot(const Vec2<A>& v) const noexcept {
		return x * v.x + y * v.y;
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 X distance(const Vec2<A>& v) const {
		return Vec2<X>(x - v.x, y - v.y).length();
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A angle(const Vec2<A>& v) const {

		arc_assert(!isNull() && !v.isNull(), "Vec2 angle with null vector");

		return Math::acos(dot(v) / (length() * v.length()));

	}


	template<CC::Arithmetic A>
	constexpr static Vec2<A> normalize(Vec2<A> v) noexcept {

		v.normalize();
		return v;

	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 static X length(Vec2<A> v) {
		return v.length();
	}


	constexpr Vec3<T> toVec3() const noexcept {
		return { x, y, 0 };
	}

	constexpr Vec4<T> toVec4() const noexcept {
		return { x, y, 0, 0 };
	}


	T x;
	T y;

};

template<CC::Arithmetic A>
Vec2(A)->Vec2<A>;

template<CC::Arithmetic A, CC::Arithmetic B>
Vec2(A, B)->Vec2<TT::CommonType<A, B>>;


template<CC::Arithmetic T>
class Vec3 {

private:

	using X = TT::PromotedType<T>;

public:

	using Type = T;

	constexpr static SizeT Size = 3;



	constexpr Vec3() noexcept : x(0), y(0), z(0) {}

	constexpr explicit Vec3(T value) noexcept : x(value), y(value), z(value) {}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	constexpr Vec3(A x, B y, C z) noexcept : x(x), y(y), z(z) {}

	template<CC::Arithmetic A>
	constexpr explicit Vec3(const Vec2<A>& v) noexcept : x(v.x), y(v.y), z(0) {}

	template<CC::Arithmetic A>
	constexpr Vec3(const Vec3<A>& v) noexcept : x(v.x), y(v.y), z(v.z) {}


	template<CC::Arithmetic A>
	constexpr Vec3& add(const Vec3<A>& v) noexcept {

		x += v.x;
		y += v.y;
		z += v.z;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec3& subtract(const Vec3<A>& v) noexcept {

		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec3& multiply(A s) noexcept {

		x *= s;
		y *= s;
		z *= s;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec3& divide(A s) noexcept {

		arc_assert(!Math::isZero(s), "Vec3 divided by 0");

		x /= s;
		y /= s;
		z /= s;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec3& compMultiply(const Vec3<A>& v) noexcept {

		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec3& compDivide(const Vec3<A>& v) noexcept {

		arc_assert(!v.isNull(), "Vec3 divided by 0");

		x /= v.x;
		y /= v.y;
		z /= v.z;

		return *this;

	}


	template<CC::Arithmetic A>
	constexpr Vec3& operator=(const Vec3<A>& v) noexcept {

		x = v.x;
		y = v.y;
		z = v.z;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec3& operator+=(const Vec3<A>& v) noexcept {
		return add(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec3& operator-=(const Vec3<A>& v) noexcept {
		return subtract(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec3& operator*=(const Vec3<A>& v) noexcept {
		return compMultiply(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec3& operator/=(const Vec3<A>& v) noexcept {
		return compDivide(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec3& operator*=(A s) noexcept {
		return multiply(s);
	}

	template<CC::Arithmetic A>
	constexpr Vec3& operator/=(A s) noexcept {
		return divide(s);
	}


	template<CC::Arithmetic A>
	constexpr bool operator==(const Vec3<A>& v) const noexcept {
		return Math::equal(x, v.x) && Math::equal(y, v.y) && Math::equal(z, v.z);
	}

	constexpr Vec3 operator-() const noexcept {
		return Vec3(-x, -y, -z);
	}


	constexpr T& operator[](SizeT index) noexcept {

		switch (index) {
			case 0:	return x;
			case 1:	return y;
			case 2:	return z;
			default:
				arc_force_assert("Vec3 access out of bounds (index=%d)", index);
				std::unreachable();
		}

	}

	constexpr const T& operator[](SizeT index) const noexcept {

		switch (index) {
			case 0:	return x;
			case 1:	return y;
			case 2:	return z;
			default:
				arc_force_assert("Vec3 access out of bounds (index=%d)", index);
				std::unreachable();
		}

	}


	constexpr X magSquared() const noexcept {
		return x * x + y * y + z * z;
	}

	ARC_CONSTEXPR_CMATH26 X length() const {
		return Math::sqrt(magSquared());
	}

	ARC_CONSTEXPR_CMATH26 void normalize() {
		divide(length());
	}

	ARC_CONSTEXPR_CMATH26 Vec3 normalized() const {
		return normalize(*this);
	}

	constexpr bool isNull() const noexcept {
		return Math::isZero(x) && Math::isZero(y) && Math::isZero(z);
	}

	constexpr bool anyNegative() const noexcept {
		return Math::less(x, 0) || Math::less(y, 0) || Math::less(z, 0);
	}


	template<CC::Arithmetic A>
	constexpr Vec3<X> cross(const Vec3<A>& v) const noexcept {
		return Vec3<X>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	template<CC::Arithmetic A>
	constexpr X dot(const Vec3<A>& v) const noexcept {
		return x * v.x + y * v.y + z * v.z;
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 X distance(const Vec3<A>& v) const {
		return Vec3<X>(x - v.x, y - v.y, z - v.z).length();
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 A angle(const Vec3<A>& v) const {

		arc_assert(!isNull() && !isNull(), "Vec3 angle with null vector");

		return Math::acos(dot(v) / (length() * v.length()));

	}


	template<CC::Arithmetic A>
	constexpr static Vec3<A> normalize(Vec3<A> v) noexcept {
		v.normalize();
		return v;
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 static X length(Vec3<A> v) {
		return v.length();
	}


	constexpr Vec2<T> toVec2() const noexcept {
		return { x, y };
	}

	constexpr Vec4<T> toVec4() const noexcept {
		return { x, y, z, 0 };
	}


	T x, y, z;

};

template<CC::Arithmetic A>
Vec3(A)->Vec3<A>;

template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
Vec3(A, B, C)->Vec3<TT::CommonType<A, B, C>>;


template<CC::Arithmetic T>
class alignas(16) Vec4 {

private:

	using X = TT::PromotedType<T>;

public:

	using Type = T;

	constexpr static SizeT Size = 4;



	constexpr Vec4() noexcept : x(0), y(0), z(0), w(0) {}

	constexpr explicit Vec4(T value) noexcept : x(value), y(value), z(value), w(value) {}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C, CC::Arithmetic D>
	constexpr Vec4(A x, B y, C z, D w) noexcept : x(x), y(y), z(z), w(w) {}

	template<CC::Arithmetic A>
	constexpr explicit Vec4(const Vec2<A>& v) noexcept : x(v.x), y(v.y), z(0), w(0) {}

	template<CC::Arithmetic A>
	constexpr explicit Vec4(const Vec3<A>& v) noexcept : x(v.x), y(v.y), z(v.z), w(0) {}

	template<CC::Arithmetic A>
	constexpr Vec4(const Vec4<A>& v) noexcept : x(v.x), y(v.y), z(v.z), w(v.w) {}


	template<CC::Arithmetic A>
	constexpr Vec4& add(const Vec4<A>& v) noexcept {

		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec4& subtract(const Vec4<A>& v) noexcept {

		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec4& multiply(A s) noexcept {

		x *= s;
		y *= s;
		z *= s;
		w *= s;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec4& divide(A s) noexcept {

		arc_assert(!Math::isZero(s), "Vec4 divided by 0");

		x /= s;
		y /= s;
		z /= s;
		w /= s;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec4& compMultiply(const Vec4<A>& v) noexcept {

		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec4& compDivide(const Vec4<A>& v) noexcept {

		arc_assert(!v.isNull(), "Vec4 divided by 0");

		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;

		return *this;

	}


	template<CC::Arithmetic A>
	constexpr Vec4& operator=(const Vec4<A>& v) noexcept {

		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Vec4& operator+=(const Vec4<A>& v) noexcept {
		return add(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec4& operator-=(const Vec4<A>& v) noexcept {
		return subtract(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec4& operator*=(const Vec4<A>& v) noexcept {
		return compMultiply(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec4& operator/=(const Vec4<A>& v) noexcept {
		return compDivide(v);
	}

	template<CC::Arithmetic A>
	constexpr Vec4& operator*=(A s) noexcept {
		return multiply(s);
	}

	template<CC::Arithmetic A>
	constexpr Vec4& operator/=(A s) noexcept {
		return divide(s);
	}


	template<CC::Arithmetic A>
	constexpr bool operator==(const Vec4<A>& v) const noexcept {
		return Math::equal(x, v.x) && Math::equal(y, v.y) && Math::equal(z, v.z) && Math::equal(w, v.w);
	}

	constexpr Vec4 operator-() const noexcept {
		return Vec4(-x, -y, -z, -w);
	}


	constexpr T& operator[](SizeT index) noexcept {

		switch (index) {
			case 0:	return x;
			case 1:	return y;
			case 2:	return z;
			case 3:	return w;
			default:
				arc_force_assert("Vec4 access out of bounds (index=%d)", index);
				std::unreachable();
		}

	}

	constexpr const T& operator[](SizeT index) const noexcept {

		switch (index) {
			case 0:	return x;
			case 1:	return y;
			case 2:	return z;
			case 3:	return w;
			default:
				arc_force_assert("Vec4 access out of bounds (index=%d)", index);
				std::unreachable();
		}

	}


	constexpr X magSquared() const noexcept {
		return x * x + y * y + z * z + w * w;
	}

	ARC_CONSTEXPR_CMATH26 T length() const {
		return Math::sqrt(magSquared());
	}

	ARC_CONSTEXPR_CMATH26 void normalize() {
		divide(length());
	}

	ARC_CONSTEXPR_CMATH26 Vec4 normalized() const {
		return normalize(*this);
	}

	constexpr bool isNull() const noexcept {
		return Math::isZero(x) && Math::isZero(y) && Math::isZero(z) && Math::isZero(w);
	}

	constexpr bool anyNegative() const noexcept {
		return Math::less(x, 0) || Math::less(y, 0) || Math::less(z, 0) || Math::less(w, 0);
	}


	template<CC::Arithmetic A>
	constexpr X dot(const Vec4<A>& v) const noexcept {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	template<CC::Arithmetic A>
	constexpr X distance(const Vec4<A>& v) const noexcept {
		return Vec4<X>(x - v.x, y - v.y, z - v.z, w - v.w).length();
	}

	template<CC::Arithmetic A>
	constexpr A angle(const Vec4<A>& v) const noexcept {

		arc_assert(!isNull() && !v.isNull(), "Vec4 angle with null vector");

		return Math::acos(dot(v) / (length() * v.length()));

	}


	template<CC::Arithmetic A>
	constexpr static Vec4<A> normalize(Vec4<A> v) noexcept {

		v.normalize();
		return v;

	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 static X length(Vec4<A> v) {
		return v.length();
	}


	constexpr Vec2<T> toVec2() const noexcept {
		return { x, y };
	}

	constexpr Vec3<T> toVec3() const noexcept {
		return { x, y, z };
	}


	T x;
	T y;
	T z;
	T w;

};

template<CC::Arithmetic A>
Vec4(A)->Vec4<A>;

template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C, CC::Arithmetic D>
Vec4(A, B, C, D)->Vec4<TT::CommonType<A, B, C, D>>;


template<CC::Vector A, CC::EqualVectorRank<A> B>
constexpr TT::CommonVectorType<A, B> operator+(A a, const B& b) noexcept {

	TT::CommonVectorType<A, B> ax = a;
	return ax += b;

}

template<CC::Vector A, CC::EqualVectorRank<A> B>
constexpr TT::CommonVectorType<A, B> operator-(A a, const B& b) noexcept {

	TT::CommonVectorType<A, B> ax = a;
	return ax -= b;

}


template<CC::Vector A, CC::EqualVectorRank<A> B>
constexpr TT::CommonVectorType<A, B> operator*(A a, const B& b) noexcept {

	TT::CommonVectorType<A, B> ax = a;
	return ax *= b;

}

template<CC::Vector A, CC::Arithmetic B>
constexpr auto operator*(A a, B b) noexcept {

	TT::SizedVector<A, TT::CommonType<typename A::Type, B>> ax = a;
	return ax *= b;

}

template<CC::Vector A, CC::Arithmetic B>
constexpr auto operator*(B b, A a) noexcept {
	return a * b;
}


template<CC::Vector A, CC::EqualVectorRank<A> B>
constexpr TT::CommonVectorType<A, B> operator/(A a, const B& b) noexcept {

	TT::CommonVectorType<A, B> ax = a;
	return ax /= b;

}

template<CC::Vector A, CC::Arithmetic B>
constexpr auto operator/(A a, B b) noexcept {

	TT::SizedVector<A, TT::CommonType<typename A::Type, B>> ax = a;
	return ax /= b;

}


template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr A& operator&=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralVector<B>) {
			a[i] &= b[i];
		} else {
			a[i] &= b;
		}

	}

	return a;

}

template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr void operator|=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralVector<B>) {
			a[i] |= b[i];
		} else {
			a[i] |= b;
		}

	}

	return a;

}

template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr void operator^=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralVector<B>) {
			a[i] ^= b[i];
		} else {
			a[i] ^= b;
		}

	}

	return a;

}


template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr A operator&(A a, const B& b) noexcept {
	return a &= b;
}

template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr A operator|(A a, const B& b) noexcept {
	return a |= b;
}

template<CC::IntegralVector A, class B> requires(CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr A operator^(A a, const B& b) noexcept {
	return a ^= b;
}

template<CC::IntegralVector A>
constexpr A operator~(A a) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {
		a[i] = ~a[i];
	}

	return a;

}


template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr A& operator<<=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralVector<B>) {
			a[i] <<= b[i];
		} else {
			a[i] <<= b;
		}

	}

	return a;

}

template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr A& operator>>=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralVector<B>) {
			a[i] >>= b[i];
		} else {
			a[i] >>= b;
		}

	}

	return a;

}

template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr A operator<<(A a, const B& b) noexcept {
	return a <<= b;
}

template<CC::IntegralVector A, class B> requires (CC::Integral<B> || (CC::IntegralVector<B> && CC::EqualVectorRank<A, B>))
constexpr A operator>>(A a, const B& b) noexcept {
	return a >>= b;
}


namespace Math {

	namespace Exact {

		template<CC::Vector V, CC::Arithmetic A>
		constexpr V lerp(V start, V end, A factor) noexcept {

			V ret;

			for (SizeT i = 0; i < V::Size; i++) {
				ret[i] = Math::Exact::lerp(start[i], end[i], factor);
			}

			return ret;

		}

	}


	template<CC::Vector V, CC::Vector U, CC::Arithmetic A>
	constexpr TT::CommonVectorType<V, U> lerp(V start, U end, A factor) noexcept {
		return start + factor * (end - start);
	}

	template<CC::Vector V, CC::Vector U, CC::Vector T>
	constexpr TT::CommonVectorType<V, U, T> inRange(V point, U lower, T upper) noexcept {

		bool condition = true;

		for (SizeT i = 0; i < V::Size; i++) {
			condition &= greaterEqual(point[i], lower[i]) && lessEqual(point[i], upper[i]);
		}

		return condition;

	}


	template<CC::Vector V>
	constexpr V abs(V vec) noexcept {

		V ret;

		for (SizeT i = 0; i < V::Size; i++) {
			ret[i] = Math::abs(vec[i]);
		}

		return ret;

	}


	template<CC::Vector V>
	constexpr V floor(V vec) noexcept {

		V ret;

		for (SizeT i = 0; i < V::Size; i++) {
			ret[i] = Math::floor(vec[i]);
		}

		return ret;

	}

	template<CC::Vector V>
	constexpr V ceil(V vec) noexcept {

		V ret;

		for (SizeT i = 0; i < V::Size; i++) {
			ret[i] = Math::ceil(vec[i]);
		}

		return ret;

	}

	template<CC::Vector V>
	constexpr V trunc(V vec) noexcept {

		V ret;

		for (SizeT i = 0; i < V::Size; i++) {
			ret[i] = Math::trunc(vec[i]);
		}

		return ret;

	}

	template<CC::Vector V>
	constexpr V round(V vec) noexcept {

		V ret;

		for (SizeT i = 0; i < V::Size; i++) {
			ret[i] = Math::round(vec[i]);
		}

		return ret;

	}

	template<CC::Vector V>
	constexpr V extrude(V vec) noexcept {

		V ret;

		for (SizeT i = 0; i < V::Size; i++) {
			ret[i] = Math::extrude(vec[i]);
		}

		return ret;

	}

}


template<CC::Vector V>
RawLog& operator<<(RawLog& log, const V& vec) {

	log << "Vec" << V::Size << "[";

	for(u32 i = 0; i < V::Size - 1; i++) {
		log << vec[i] <<  ", ";
	}

	log << vec[V::Size - 1] << "]";

	return log;

}


ARC_DEFINE_ARITHMETIC_ALIASES(0, Vec2, Vec2)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Vec3, Vec3)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Vec4, Vec4)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Vec2, Vector2)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Vec3, Vector3)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Vec4, Vector4)
