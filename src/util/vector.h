#pragma once

#include <cmath>

#include "util/math.h"
#include "util/assert.h"



template<Arithmetic T>
class Vec2 {

public:

	inline constexpr Vec2() : x(T(0)), y(T(0)) {}

	template<Arithmetic A>
	inline constexpr explicit Vec2(A value) : x(static_cast<T>(value)), y(static_cast<T>(value)) {}

	template<Arithmetic A, Arithmetic B>
	inline constexpr Vec2(A x, B y) : x(static_cast<T>(x)), y(static_cast<T>(y)) {}


	template<Arithmetic A>
	inline void add(const Vec2<A>& v) {
		x += v.x;
		y += v.y;
	}

	template<Arithmetic A>
	inline void sub(const Vec2<A>& v) {
		x -= v.x;
		y -= v.y;
	}

	template<Arithmetic A>
	inline void mul(A s) {
		x *= s;
		y *= s;
	}

	template<Arithmetic A>
	inline void div(A s) {
		arc_assert(!Math::isZero(s), "Vec2 divided by 0");
		x /= s;
		y /= s;
	}

	template<Arithmetic A>
	inline Vec2& operator=(const Vec2<A>& v) {
		x = v.x;
		y = v.y;
		return *this;
	}

	template<Arithmetic A>
	inline Vec2& operator+=(const Vec2<A>& v) {
		add(v);
		return *this;
	}

	template<Arithmetic A>
	inline Vec2& operator-=(const Vec2<A>& v) {
		sub(v);
		return *this;
	}

	template<Arithmetic A>
	inline Vec2& operator*=(A s) {
		mul(s);
		return *this;
	}

	template<Arithmetic A>
	inline Vec2& operator/=(A s) {
		div(s);
		return *this;
	}

	template<Arithmetic A>
	inline bool operator==(const Vec2<A>& v) const {
		return Math::isEqual(x, v.x) && Math::isEqual(y, v.y);
	}

	template<Arithmetic A>
	inline bool operator!=(const Vec2<A>& v) const {
		return !(*this == v);
	}

	inline Vec2 operator-() const {
		return Vec2(-x, -y);
	}


	inline auto length() const {
		return Math::sqrt(x * x + y * y);
	}

	inline void normalize() {
		div(length());
	}

	template<Arithmetic A>
	inline auto dot(const Vec2<A>& v) const {
		return x * v.x + y * v.y;
	}

	template<Arithmetic A>
	inline auto distance(const Vec2<A>& v) const {
		return Vec2<decltype(x - v.x)>(x - v.x, y - v.y).length();
	}

	template<Arithmetic A>
	inline auto angle(const Vec2<A>& v) const {
		arc_assert(!Math::isZero(length()) && !Math::isZero(v.length()), "Vec2 angle with null vector");
		return Math::acos(dot(v) / (length() * v.length()));
	}


	union {
		struct {
			T x, y;
		};
		T n[2];
	};

};



template<Arithmetic T>
class Vec3 {

public:

	inline constexpr Vec3() : x(T(0)), y(T(0)), z(T(0)) {}

	template<Arithmetic A>
	inline constexpr explicit Vec3(A value) : x(static_cast<T>(value)), y(static_cast<T>(value)), z(static_cast<T>(value)) {}

	template<Arithmetic A, Arithmetic B, Arithmetic C>
	inline constexpr Vec3(A x, B y, C z) : x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)) {}
	
	template<Arithmetic A>
	inline constexpr explicit Vec3(const Vec2<A>& v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(T(0)) {}


	template<Arithmetic A>
	inline void add(const Vec3<A>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}

	template<Arithmetic A>
	inline void sub(const Vec3<A>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	template<Arithmetic A>
	inline void mul(A s) {
		x *= s;
		y *= s;
		z *= s;
	}

	template<Arithmetic A>
	inline void div(A s) {
		arc_assert(!Math::isZero(s), "Vec3 divided by 0");
		x /= s;
		y /= s;
		z /= s;
	}

	template<Arithmetic A>
	inline Vec3& operator=(const Vec3<A>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	template<Arithmetic A>
	inline Vec3& operator+=(const Vec3<A>& v) {
		add(v);
		return *this;
	}

	template<Arithmetic A>
	inline Vec3& operator-=(const Vec3<A>& v) {
		sub(v);
		return *this;
	}

	template<Arithmetic A>
	inline Vec3& operator*=(A s) {
		mul(s);
		return *this;
	}

	template<Arithmetic A>
	inline Vec3& operator/=(A s) {
		div(s);
		return *this;
	}

	template<Arithmetic A>
	inline bool operator==(const Vec3<A>& v) const {
		return Math::isEqual(x, v.x) && Math::isEqual(y, v.y) && Math::isEqual(z, v.z);

	}

	template<Arithmetic A>
	inline bool operator!=(const Vec3<A>& v) const {
		return !(*this == v);
	}

	inline Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}


	inline auto length() const {
		return Math::sqrt(x * x + y * y + z * z);
	}

	inline void normalize() {
		div(length());
	}

	template<Arithmetic A>
	inline auto cross(const Vec3<A>& v) const {
		return Vec3<decltype(y* v.z - z * v.y)>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	template<Arithmetic A>
	inline auto dot(const Vec3<A>& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	template<Arithmetic A>
	inline auto distance(const Vec3<A>& v) const {
		return Vec3<decltype(x - v.x)>(x - v.x, y - v.y, z - v.z).length();
	}

	template<Arithmetic A>
	inline auto angle(const Vec3<A>& v) const {
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

	inline constexpr Vec4() : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}

	template<Arithmetic A>
	inline constexpr explicit Vec4(A value) : x(static_cast<T>(value)), y(static_cast<T>(value)), z(static_cast<T>(value)), w(static_cast<T>(value)) {}

	template<Arithmetic A, Arithmetic B, Arithmetic C, Arithmetic D>
	inline constexpr Vec4(A x, B y, C z, D w) : x(static_cast<T>(x)), y(static_cast<T>(y)), z(static_cast<T>(z)), w(static_cast<T>(w)) {}

	template<Arithmetic A>
	inline constexpr explicit Vec4(const Vec2<A>& v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(T(0)), w(T(0)) {}

	template<Arithmetic A>
	inline constexpr explicit Vec4(const Vec3<A>& v) : x(static_cast<T>(v.x)), y(static_cast<T>(v.y)), z(static_cast<T>(v.z)), w(T(0)) {}


	template<Arithmetic A>
	inline void add(const Vec4<A>& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

	template<Arithmetic A>
	inline void sub(const Vec4<A>& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
	}

	template<Arithmetic A>
	inline void mul(A s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	template<Arithmetic A>
	inline void div(A s) {
		arc_assert(!Math::isZero(s), "Vec4 divided by 0");
		x /= s;
		y /= s;
		z /= s;
		w /= s;
	}


	template<Arithmetic A>
	inline Vec4& operator=(const Vec4<A>& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	template<Arithmetic A>
	inline Vec4& operator+=(const Vec4<A>& v) {
		add(v);
		return *this;
	}

	template<Arithmetic A>
	inline Vec4& operator-=(const Vec4<A>& v) {
		sub(v);
		return *this;
	}

	template<Arithmetic A>
	inline Vec4& operator*=(A s) {
		mul(s);
		return *this;
	}

	template<Arithmetic A>
	inline Vec4& operator/=(A s) {
		div(s);
		return *this;
	}

	template<Arithmetic A>
	inline bool operator==(const Vec4<A>& v) const {
		return Math::isEqual(x, v.x) && Math::isEqual(y, v.y) && Math::isEqual(z, v.z) && Math::isEqual(w, v.w);
	}

	template<Arithmetic A>
	inline bool operator!=(const Vec4<A>& v) const {
		return !(*this == v);
	}

	inline Vec4 operator-() const {
		return Vec4(-x, -y, -z, -w);
	}


	inline auto length() const {
		return Math::sqrt(x * x + y * y + z * z + w * w);
	}

	inline void normalize() {
		div(length());
	}

	template<Arithmetic A>
	inline auto dot(const Vec4<A>& v) const {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	template<Arithmetic A>
	inline auto distance(const Vec4<A>& v) const {
		return Vec4<decltype(x - v.x)>(x - v.x, y - v.y, z - v.z, w - v.w).length();
	}

	template<Arithmetic A>
	inline auto angle(const Vec4<A>& v) const {
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
inline Vector<A> operator+(Vector<A> a, const Vector<B>& b) requires (std::is_same_v<Vector<A>, Vec2<A>> || std::is_same_v<Vector<A>, Vec3<A>> || std::is_same_v<Vector<A>, Vec4<A>>){
	a += b;
	return a;
}

template<Arithmetic A, Arithmetic B, template<Arithmetic> class Vector>
inline Vector<A> operator-(Vector<A> a, const Vector<B>& b) requires (std::is_same_v<Vector<A>, Vec2<A>> || std::is_same_v<Vector<A>, Vec3<A>> || std::is_same_v<Vector<A>, Vec4<A>>) {
	a -= b;
	return a;
}

template<Arithmetic A, Arithmetic B, template<Arithmetic> class Vector>
inline Vector<A> operator*(Vector<A> a, B s) requires (std::is_same_v<Vector<A>, Vec2<A>> || std::is_same_v<Vector<A>, Vec3<A>> || std::is_same_v<Vector<A>, Vec4<A>>) {
	a *= s;
	return a;
}

template<Arithmetic A, Arithmetic B, template<Arithmetic> class Vector>
inline Vector<A> operator/(Vector<A> a, B s) requires (std::is_same_v<Vector<A>, Vec2<A>> || std::is_same_v<Vector<A>, Vec3<A>> || std::is_same_v<Vector<A>, Vec4<A>>) {
	a /= s;
	return a;
}


#define VECTOR_DEFINE_NDTS(name, dim, type, suffix) typedef Vec##dim##<##type##> name##dim##suffix##;

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