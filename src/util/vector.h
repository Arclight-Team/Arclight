#pragma once

#include <cmath>

#include "util/math.h"
#include "util/assert.h"



template<Arithmetic T>
class Vec2 {

public:

	constexpr Vec2() : x(T(0)), y(T(0)) {}

	template<Arithmetic A>
	constexpr explicit Vec2(A value) : x(value), y(value) {}

	template<Arithmetic A, Arithmetic B>
	constexpr Vec2(A x, B y) : x(x), y(y) {}

	template<Arithmetic A>
	constexpr Vec2(const Vec2<A>& v) : x(v.x), y(v.y) {}


	template<Arithmetic A>
	constexpr void add(const Vec2<A>& v) {
		x += v.x;
		y += v.y;
	}

	template<Arithmetic A>
	constexpr void sub(const Vec2<A>& v) {
		x -= v.x;
		y -= v.y;
	}

	template<Arithmetic A>
	constexpr void mul(A s) {
		x *= s;
		y *= s;
	}

	template<Arithmetic A>
	constexpr void div(A s) {
		arc_assert(!Math::isZero(s), "Vec2 divided by 0");
		x /= s;
		y /= s;
	}

	template<Arithmetic A>
	constexpr Vec2& operator=(const Vec2<A>& v) {
		x = v.x;
		y = v.y;
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec2& operator+=(const Vec2<A>& v) {
		add(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec2& operator-=(const Vec2<A>& v) {
		sub(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec2& operator*=(A s) {
		mul(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec2& operator/=(A s) {
		div(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr bool operator==(const Vec2<A>& v) const {
		return Math::isEqual(x, v.x) && Math::isEqual(y, v.y);
	}

	template<Arithmetic A>
	constexpr bool operator!=(const Vec2<A>& v) const {
		return !(*this == v);
	}

	constexpr Vec2 operator-() const {
		return Vec2(-x, -y);
	}

	constexpr T& operator[](u32 index) {
		
		switch (index) {

			case 0:	return x;
			case 1:	return y;
			default:
				arc_assert(false, "Vec2 access out of bounds (index=%d)", index);
				return x;

		}

	}


	constexpr auto magSquared() const {
		return x * x + y * y;
	}

	constexpr auto length() const {
		return Math::sqrt(magSquared);
	}

	constexpr void normalize() {
		div(length());
	}

	template<Arithmetic A>
	constexpr auto dot(const Vec2<A>& v) const {
		return x * v.x + y * v.y;
	}

	template<Arithmetic A>
	constexpr auto distance(const Vec2<A>& v) const {
		return { x - v.x, y - v.y }.length();
	}

	template<Arithmetic A>
	constexpr auto angle(const Vec2<A>& v) const {
		arc_assert(!Math::isZero(length()) && !Math::isZero(v.length()), "Vec2 angle with null vector");
		return Math::acos(dot(v) / (length() * v.length()));
	}


	T x, y;

};



template<Arithmetic T>
class Vec3 {

public:

	constexpr Vec3() : x(T(0)), y(T(0)), z(T(0)) {}

	template<Arithmetic A>
	constexpr explicit Vec3(A value) : x(value), y(value), z(value) {}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr Vec3(A x, B y, C z) : x(x), y(y), z(z) {}
	
	template<Arithmetic A>
	constexpr explicit Vec3(const Vec2<A>& v) : x(v.x), y(v.y), z(T(0)) {}

	template<Arithmetic A>
	constexpr Vec3(const Vec3<A>& v) : x(v.x), y(v.y), z(v.z) {}


	template<Arithmetic A>
	constexpr void add(const Vec3<A>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}

	template<Arithmetic A>
	constexpr void sub(const Vec3<A>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	template<Arithmetic A>
	constexpr void mul(A s) {
		x *= s;
		y *= s;
		z *= s;
	}

	template<Arithmetic A>
	constexpr void div(A s) {
		arc_assert(!Math::isZero(s), "Vec3 divided by 0");
		x /= s;
		y /= s;
		z /= s;
	}

	template<Arithmetic A>
	constexpr Vec3& operator=(const Vec3<A>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec3& operator+=(const Vec3<A>& v) {
		add(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec3& operator-=(const Vec3<A>& v) {
		sub(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec3& operator*=(A s) {
		mul(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec3& operator/=(A s) {
		div(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr bool operator==(const Vec3<A>& v) const {
		return Math::isEqual(x, v.x) && Math::isEqual(y, v.y) && Math::isEqual(z, v.z);

	}

	template<Arithmetic A>
	constexpr bool operator!=(const Vec3<A>& v) const {
		return !(*this == v);
	}

	constexpr Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}

	constexpr T& operator[](u32 index) {

		switch (index) {

			case 0:	return x;
			case 1:	return y;
			case 2:	return z;
			default:
				arc_assert(false, "Vec3 access out of bounds (index=%d)", index);
				return x;

		}

	}


	constexpr auto magSquared() const {
		return x * x + y * y + z * z;
	}

	constexpr auto length() const {
		return Math::sqrt(magSquared);
	}

	constexpr void normalize() {
		div(length());
	}

	template<Arithmetic A>
	constexpr auto cross(const Vec3<A>& v) const {
		return Vec3<decltype(y* v.z - z * v.y)>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	template<Arithmetic A>
	constexpr auto dot(const Vec3<A>& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	template<Arithmetic A>
	constexpr auto distance(const Vec3<A>& v) const {
		return { x - v.x, y - v.y, z - v.z }.length();
	}

	template<Arithmetic A>
	constexpr auto angle(const Vec3<A>& v) const {
		arc_assert(!Math::isZero(length()) && !Math::isZero(v.length()), "Vec3 angle with null vector");
		return Math::acos(dot(v) / (length() * v.length()));
	}


	union {
		struct {
			T x, y, z;
		};
		struct {
			T r, g, b;
		};
		T n[3];
	};

};



template<Arithmetic T>
class Vec4 {

public:

	constexpr Vec4() : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}

	template<Arithmetic A>
	constexpr explicit Vec4(A value) : x(value), y(value), z(value), w(value) {}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D>
	constexpr Vec4(A x, B y, C z, D w) : x(x), y(y), z(z), w(w) {}

	template<Arithmetic A>
	constexpr explicit Vec4(const Vec2<A>& v) : x(v.x), y(v.y), z(T(0)), w(T(0)) {}

	template<Arithmetic A>
	constexpr explicit Vec4(const Vec3<A>& v) : x(v.x), y(v.y), z(v.z), w(T(0)) {}

	template<Arithmetic A>
	constexpr Vec4(const Vec4<A>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}


	template<Arithmetic A>
	constexpr void add(const Vec4<A>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

	template<Arithmetic A>
	constexpr void sub(const Vec4<A>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
	}

	template<Arithmetic A>
	constexpr void mul(A s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	template<Arithmetic A>
	constexpr void div(A s) {
		arc_assert(!Math::isZero(s), "Vec4 divided by 0");
		x /= s;
		y /= s;
		z /= s;
		w /= s;
	}


	template<Arithmetic A>
	constexpr Vec4& operator=(const Vec4<A>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec4& operator+=(const Vec4<A>& v) {
		add(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec4& operator-=(const Vec4<A>& v) {
		sub(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec4& operator*=(A s) {
		mul(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec4& operator/=(A s) {
		div(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr bool operator==(const Vec4<A>& v) const {
		return Math::isEqual(x, v.x) && Math::isEqual(y, v.y) && Math::isEqual(z, v.z) && Math::isEqual(w, v.w);
	}

	template<Arithmetic A>
	constexpr bool operator!=(const Vec4<A>& v) const {
		return !(*this == v);
	}

	constexpr Vec4 operator-() const {
		return Vec4(-x, -y, -z, -w);
	}

	constexpr T& operator[](u32 index) {

		switch (index) {

			case 0:	return x;
			case 1:	return y;
			case 2:	return z;
			case 3:	return w;
			default:
				arc_assert(false, "Vec4 access out of bounds (index=%d)", index);
				return x;

		}

	}


	constexpr auto magSquared() const {
		return x * x + y * y + z * z + w * w;
	}

	constexpr auto length() const {
		return Math::sqrt(magSquared);
	}

	constexpr void normalize() {
		div(length());
	}

	template<Arithmetic A>
	constexpr auto dot(const Vec4<A>& v) const {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	template<Arithmetic A>
	constexpr auto distance(const Vec4<A>& v) const {
		return { x - v.x, y - v.y, z - v.z, w - v.w }.length();
	}

	template<Arithmetic A>
	constexpr auto angle(const Vec4<A>& v) const {
		arc_assert(!Math::isZero(length()) && !Math::isZero(v.length()), "Vec4 angle with null vector");
		return Math::acos(dot(v) / (length() * v.length()));
	}


	union {
		struct {
			T x, y, z, w;
		};
		struct {
			T r, g, b, a;
		};
		T n[4];
	};

};




template<Arithmetic A, Arithmetic B, template<Arithmetic> class Vector>
constexpr Vector<A> operator+(Vector<A> a, const Vector<B>& b) requires (std::is_same_v<Vector<A>, Vec2<A>> || std::is_same_v<Vector<A>, Vec3<A>> || std::is_same_v<Vector<A>, Vec4<A>>){
	a += b;
	return a;
}

template<Arithmetic A, Arithmetic B, template<Arithmetic> class Vector>
constexpr Vector<A> operator-(Vector<A> a, const Vector<B>& b) requires (std::is_same_v<Vector<A>, Vec2<A>> || std::is_same_v<Vector<A>, Vec3<A>> || std::is_same_v<Vector<A>, Vec4<A>>) {
	a -= b;
	return a;
}

template<Arithmetic A, Arithmetic B, template<Arithmetic> class Vector>
constexpr Vector<A> operator*(Vector<A> a, B s) requires (std::is_same_v<Vector<A>, Vec2<A>> || std::is_same_v<Vector<A>, Vec3<A>> || std::is_same_v<Vector<A>, Vec4<A>>) {
	a *= s;
	return a;
}

template<Arithmetic A, Arithmetic B, template<Arithmetic> class Vector>
constexpr Vector<A> operator/(Vector<A> a, B s) requires (std::is_same_v<Vector<A>, Vec2<A>> || std::is_same_v<Vector<A>, Vec3<A>> || std::is_same_v<Vector<A>, Vec4<A>>) {
	a /= s;
	return a;
}


#define VECTOR_DEFINE_NDTS(name, dim, type, suffix) typedef Vec##dim<type> name##dim##suffix;

#define VECTOR_DEFINE_ND(name, dim) \
	VECTOR_DEFINE_NDTS(name, dim, float, f) \
	VECTOR_DEFINE_NDTS(name, dim, double, d) \
	VECTOR_DEFINE_NDTS(name, dim, long double, ld) \
	VECTOR_DEFINE_NDTS(name, dim, i8, c) \
	VECTOR_DEFINE_NDTS(name, dim, i16, s) \
	VECTOR_DEFINE_NDTS(name, dim, i32, i) \
	VECTOR_DEFINE_NDTS(name, dim, i64, l) \
	VECTOR_DEFINE_NDTS(name, dim, u8, uc) \
	VECTOR_DEFINE_NDTS(name, dim, u16, us) \
	VECTOR_DEFINE_NDTS(name, dim, u32, ui) \
	VECTOR_DEFINE_NDTS(name, dim, u64, ul)

#define VECTOR_DEFINE_N(name) \
	VECTOR_DEFINE_ND(name, 2) \
	VECTOR_DEFINE_ND(name, 3) \
	VECTOR_DEFINE_ND(name, 4)

#define VECTOR_DEFINE \
	VECTOR_DEFINE_N(Vec) \
	VECTOR_DEFINE_N(Vector)

VECTOR_DEFINE

#undef VECTOR_DEFINE
#undef VECTOR_DEFINE_N
#undef VECTOR_DEFINE_ND
#undef VECTOR_DEFINE_NDTS