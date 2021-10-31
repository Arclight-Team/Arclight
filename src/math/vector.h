#pragma once

#include "math/math.h"
#include "util/assert.h"
#include "random.h"
#include "util/typetraits.h"



template<Arithmetic T>
class Vec3;

template<Arithmetic T>
class Vec4;


template<Arithmetic T>
class Vec2 {

public:

	using Type = T;
	constexpr static u32 Size = 2;
	
	template<Arithmetic A>
	using Untyped = Vec2<A>;


	constexpr Vec2() : x(T(0)), y(T(0)) {}

	template<Arithmetic A>
	constexpr explicit Vec2(A value) : x(T(value)), y(T(value)) {}

	template<Arithmetic A, Arithmetic B>
	constexpr Vec2(A x, B y) : x(T(x)), y(T(y)) {}

	template<Arithmetic A>
	constexpr Vec2(const Vec2<A>& v) : x(T(v.x)), y(T(v.y)) {}


	template<Arithmetic A>
	constexpr void add(const Vec2<A>& v) {
		x += v.x;
		y += v.y;
	}

	template<Arithmetic A>
	constexpr void subtract(const Vec2<A>& v) {
		x -= v.x;
		y -= v.y;
	}

	template<Arithmetic A>
	constexpr void multiply(A s) {
		x *= s;
		y *= s;
	}

	template<Arithmetic A>
	constexpr void divide(A s) {
		arc_assert(!Math::isZero(s), "Vec2 divided by 0");
		x /= s;
		y /= s;
	}

	template<Arithmetic A>
	constexpr void compMultiply(const Vec2<A>& v) {
		x *= v.x;
		y *= v.y;
	}

	template<Arithmetic A>
	constexpr void compDivide(const Vec2<A>& v) {
		x /= v.x;
		y /= v.y;
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
		subtract(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec2& operator*=(A s) {
		multiply(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec2& operator/=(A s) {
		divide(s);
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
			arc_force_assert("Vec2 access out of bounds (index=%d)", index);
			return x;

		}

	}

	constexpr const T& operator[](u32 index) const {

		switch (index) {

		case 0:	return x;
		case 1:	return y;
		default:
			arc_force_assert("Vec2 access out of bounds (index=%d)", index);
			return x;

		}

	}


	constexpr auto magSquared() const {
		return x * x + y * y;
	}

	constexpr auto length() const {
		return Math::sqrt(magSquared());
	}

	constexpr bool isNull() const {
		return Math::isZero(x) && Math::isZero(y);
	}

	constexpr void normalize() {
		divide(length());
	}

	constexpr auto normalized() const {
		return normalize(*this);
	}

	template<Arithmetic A>
	constexpr auto dot(const Vec2<A>& v) const {
		return x * v.x + y * v.y;
	}

	template<Arithmetic A>
	constexpr auto distance(const Vec2<A>& v) const {
		return Vec2<decltype(x - v.x)>(x - v.x, y - v.y).length();
	}

	template<Arithmetic A>
	constexpr auto angle(const Vec2<A>& v) const {
		arc_assert(!Math::isZero(length()) && !Math::isZero(v.length()), "Vec2 angle with null vector");
		return Math::acos(dot(v) / (length() * v.length()));
	}

	template<Arithmetic A>
	constexpr static Vec2<A> normalize(Vec2<A> v) {
		v.normalize();
		return v;
	}

	constexpr static Vec2 randomUnit() {

		auto& random = Random::getRandom();

		Vec2 v;
		v.x = random.getInt();
		v.y = random.getInt();
		v.normalize();

		return v;

	}

	constexpr Vec3<T> toVec3() const {
		return { x, y, 0 };
	}

	constexpr Vec4<T> toVec4() const {
		return { x, y, 0, 1 };
	}


	T x, y;

};



template<Arithmetic T>
class Vec3 {

public:

	using Type = T;
	constexpr static u32 Size = 3;
	
	template<Arithmetic A>
	using Untyped = Vec3<A>;


	constexpr Vec3() : x(T(0)), y(T(0)), z(T(0)) {}

	template<Arithmetic A>
	constexpr explicit Vec3(A value) : x(T(value)), y(T(value)), z(T(value)) {}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	constexpr Vec3(A x, B y, C z) : x(T(x)), y(T(y)), z(T(z)) {}

	template<Arithmetic A>
	constexpr explicit Vec3(const Vec2<A>& v) : x(T(v.x)), y(T(v.y)), z(T(0)) {}

	template<Arithmetic A>
	constexpr Vec3(const Vec3<A>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)) {}


	template<Arithmetic A>
	constexpr void add(const Vec3<A>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}

	template<Arithmetic A>
	constexpr void subtract(const Vec3<A>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	template<Arithmetic A>
	constexpr void multiply(A s) {
		x *= s;
		y *= s;
		z *= s;
	}

	template<Arithmetic A>
	constexpr void divide(A s) {
		arc_assert(!Math::isZero(s), "Vec3 divided by 0");
		x /= s;
		y /= s;
		z /= s;
	}

	template<Arithmetic A>
	constexpr void compMultiply(const Vec3<A>& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	template<Arithmetic A>
	constexpr void compDivide(const Vec3<A>& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
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
		subtract(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec3& operator*=(A s) {
		multiply(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec3& operator/=(A s) {
		divide(s);
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
			arc_force_assert("Vec3 access out of bounds (index=%d)", index);
			return x;

		}

	}

	constexpr T operator[](u32 index) const {

		switch (index) {

		case 0:	return x;
		case 1:	return y;
		case 2:	return z;
		default:
			arc_force_assert("Vec3 access out of bounds (index=%d)", index);
			return x;

		}

	}


	constexpr auto magSquared() const {
		return x * x + y * y + z * z;
	}

	constexpr auto length() const {
		return Math::sqrt(magSquared());
	}

	constexpr bool isNull() const {
		return Math::isZero(x) && Math::isZero(y) && Math::isZero(z);
	}

	constexpr void normalize() {
		divide(length());
	}

	constexpr auto normalized() const {
		return normalize(*this);
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
		return Vec3<decltype(x - v.x)>(x - v.x, y - v.y, z - v.z).length();
	}

	template<Arithmetic A>
	constexpr auto angle(const Vec3<A>& v) const {
		arc_assert(!Math::isZero(length()) && !Math::isZero(v.length()), "Vec3 angle with null vector");
		return Math::acos(dot(v) / (length() * v.length()));
	}

	template<Arithmetic A>
	constexpr static Vec3<A> normalize(Vec3<A> v) {
		v.normalize();
		return v;
	}

	constexpr static Vec3 randomUnit() {

		auto& random = Random::getRandom();

		Vec3 v;
		v.x = random.getInt();
		v.y = random.getInt();
		v.z = random.getInt();
		v.normalize();

		return v;

	}

	constexpr Vec2<T> toVec2() const {
		return { x, y };
	}

	constexpr Vec4<T> toVec4() const {
		return { x, y, z, 1 };
	}


	T x, y, z;

};



template<Arithmetic T>
class Vec4 {

public:

	using Type = T;
	constexpr static u32 Size = 4;

	template<Arithmetic A>
	using Untyped = Vec4<A>;


	constexpr Vec4() : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}

	template<Arithmetic A>
	constexpr explicit Vec4(A value) : x(T(value)), y(T(value)), z(T(value)), w(T(value)) {}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D>
	constexpr Vec4(A x, B y, C z, D w) : x(T(x)), y(T(y)), z(T(z)), w(T(w)) {}

	template<Arithmetic A>
	constexpr explicit Vec4(const Vec2<A>& v) : x(T(v.x)), y(T(v.y)), z(T(0)), w(T(0)) {}

	template<Arithmetic A>
	constexpr explicit Vec4(const Vec3<A>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), w(T(0)) {}

	template<Arithmetic A>
	constexpr Vec4(const Vec4<A>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), w(T(v.w)) {}


	template<Arithmetic A>
	constexpr void add(const Vec4<A>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

	template<Arithmetic A>
	constexpr void subtract(const Vec4<A>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
	}

	template<Arithmetic A>
	constexpr void multiply(A s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	template<Arithmetic A>
	constexpr void divide(A s) {
		arc_assert(!Math::isZero(s), "Vec4 divided by 0");
		x /= s;
		y /= s;
		z /= s;
		w /= s;
	}

	template<Arithmetic A>
	constexpr void compMultiply(const Vec4<A>& v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;
		w *= v.w;
	}

	template<Arithmetic A>
	constexpr void compDivide(const Vec4<A>& v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;
		w /= v.w;
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
		subtract(v);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec4& operator*=(A s) {
		multiply(s);
		return *this;
	}

	template<Arithmetic A>
	constexpr Vec4& operator/=(A s) {
		divide(s);
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
			arc_force_assert("Vec4 access out of bounds (index=%d)", index);
			return x;

		}

	}

	constexpr T operator[](u32 index) const {

		switch (index) {

		case 0:	return x;
		case 1:	return y;
		case 2:	return z;
		case 3:	return w;
		default:
			arc_force_assert("Vec4 access out of bounds (index=%d)", index);
			return x;

		}

	}


	constexpr auto magSquared() const {
		return x * x + y * y + z * z + w * w;
	}

	constexpr auto length() const {
		return Math::sqrt(magSquared());
	}

	constexpr bool isNull() const {
		return Math::isZero(x) && Math::isZero(y) && Math::isZero(z) && Math::isZero(w);
	}

	constexpr void normalize() {
		divide(length());
	}

	constexpr auto normalized() const {
		return normalize(*this);
	}

	template<Arithmetic A>
	constexpr auto dot(const Vec4<A>& v) const {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	template<Arithmetic A>
	constexpr auto distance(const Vec4<A>& v) const {
		return Vec4<decltype(x - v.x)>(x - v.x, y - v.y, z - v.z, w - v.w).length();
	}

	template<Arithmetic A>
	constexpr auto angle(const Vec4<A>& v) const {
		arc_assert(!Math::isZero(length()) && !Math::isZero(v.length()), "Vec4 angle with null vector");
		return Math::acos(dot(v) / (length() * v.length()));
	}

	template<Arithmetic A>
	constexpr static Vec4<A> normalize(Vec4<A> v) {
		v.normalize();
		return v;
	}

	constexpr static Vec4 randomUnit() {

		auto& random = Random::getRandom();

		Vec4 v;
		v.x = random.getInt();
		v.y = random.getInt();
		v.z = random.getInt();
		v.w = random.getInt();
		v.normalize();

		return v;

	}

	constexpr Vec2<T> toVec2() const {
		return { x, y };
	}

	constexpr Vec3<T> toVec3() const {
		return { x, y, z };
	}


	T x, y, z, w;

};



template<class T>
concept Vector = TT::IsAnyOf<T, Vec2<typename T::Type>, Vec3<typename T::Type>, Vec4<typename T::Type>>;



template<Vector A, Vector B>
constexpr auto operator+(A a, const B& b) {
	typename A::template Untyped<decltype(a[0] + b[0])> ax = a;
	ax += b;
	return ax;
}

template<Vector A, Vector B>
constexpr auto operator-(A a, const B& b) {
	typename A::template Untyped<decltype(a[0] - b[0])> ax = a;
	ax -= b;
	return ax;
}

template<Vector A, Vector B>
constexpr auto operator*(A a, const B& b) {
	typename A::template Untyped<decltype(a[0] - b[0])> ax = a;
	ax.compMultiply(b);
	return ax;
}

template<Vector A, Arithmetic B>
constexpr auto operator*(A a, B b) {
	typename A::template Untyped<decltype(a[0] * b)> ax = a;
	ax *= b;
	return ax;
}

template<Vector A, Arithmetic B>
constexpr auto operator*(B b, A a) {
	typename A::template Untyped<decltype(a[0] * b)> ax = a;
	ax *= b;
	return ax;
}

template<Vector A, Vector B>
constexpr auto operator/(A a, const B& b) {
	typename A::template Untyped<decltype(a[0] - b[0])> ax = a;
	ax.compDivide(b);
	return ax;
}

template<Vector A, Arithmetic B>
constexpr auto operator/(A a, B b) {
	typename A::template Untyped<decltype(a[0] / b)> ax = a;
	ax /= b;
	return ax;
}



namespace Math {

	template<Vector V, class Factor = decltype(std::declval<typename V::Type>() * std::declval<float>())>
	constexpr auto lerp(V start, V end, Factor factor) {
		return start + factor * (end - start);
	}

}



#define VECTOR_DEFINE_NDTS(name, dim, type, suffix) typedef Vec##dim<type> name##dim##suffix;

#define VECTOR_DEFINE_ND(name, dim) \
	VECTOR_DEFINE_NDTS(name, dim, bool, b) \
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
	VECTOR_DEFINE_NDTS(name, dim, u64, ul) \
	VECTOR_DEFINE_NDTS(name, dim, ARC_STD_FLOAT_TYPE, x)

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
