#pragma once

#include <cmath>

#include "util/math.h"
#include "util/assert.h"


class Vec2 {

public:

	inline constexpr Vec2() : x(0), y(0) {}
	inline constexpr explicit Vec2(double value) : x(value), y(value) {}
	inline constexpr Vec2(double x, double y) : x(x), y(y) {}


	inline void add(const Vec2& v) {
		x += v.x;
		y += v.y;
	}

	inline void sub(const Vec2& v) {
		x -= v.x;
		y -= v.y;
	}

	inline void mul(double s) {
		x *= s;
		y *= s;
	}

	inline void div(double s) {
		arc_assert(abs(s) > Math::epsilon, "Vec2 divided by 0");
		x /= s;
		y /= s;
	}


	inline Vec2& operator=(const Vec2& v) {
		x = v.x;
		y = v.y;
		return *this;
	}

	inline Vec2& operator+=(const Vec2& v) {
		add(v);
		return *this;
	}

	inline Vec2& operator-=(const Vec2& v) {
		sub(v);
		return *this;
	}

	inline Vec2& operator*=(double s) {
		mul(s);
		return *this;
	}

	inline Vec2& operator/=(double s) {
		div(s);
		return *this;
	}

	inline bool operator==(const Vec2& v) const {
		if (std::abs(x - v.x) < Math::epsilon && std::abs(y - v.y) < Math::epsilon) {
			return true;
		}
		return false;
	}

	inline bool operator!=(const Vec2& v) const {
		return !(*this == v);
	}

	inline Vec2 operator-() const {
		return Vec2(-x, -y);
	}



	inline double length() const {
		return std::sqrt(x * x + y * y);
	}


	inline void normalize() {
		div(length());
	}

	inline double dot(const Vec2& v) const {
		return x * v.x + y * v.y;
	}

	inline double distance(const Vec2& v) const {
		return Vec2(x - v.x, y - v.y).length();
	}

	inline double angle(const Vec2& v) const {
		arc_assert((length() > Math::epsilon) && (v.length() > Math::epsilon), "Vec2 angle with null vector");
		return std::acos(dot(v) / (length() * v.length()));
	}


	double x, y;

};


class Vec3 {

public:

	inline constexpr Vec3() : x(0), y(0), z(0) {}
	inline constexpr explicit Vec3(double value) : x(value), y(value), z(value) {}
	inline constexpr explicit Vec3(const Vec2& v) : x(v.x), y(v.y), z(0) {}
	inline constexpr Vec3(double x, double y, double z) : x(x), y(y), z(z) {}


	inline void add(const Vec3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
	}

	inline void sub(const Vec3& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	inline void mul(double s) {
		x *= s;
		y *= s;
		z *= s;
	}

	inline void div(double s) {
		arc_assert(s != 0.0, "Vec3 divided by 0");
		x /= s;
		y /= s;
		z /= s;
	}


	inline Vec3& operator=(const Vec3& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	inline Vec3& operator+=(const Vec3& v) {
		add(v);
		return *this;
	}

	inline Vec3& operator-=(const Vec3& v) {
		sub(v);
		return *this;
	}

	inline Vec3& operator*=(double s) {
		mul(s);
		return *this;
	}

	inline Vec3& operator/=(double s) {
		div(s);
		return *this;
	}

	inline bool operator==(const Vec3& v) const {
		if (std::abs(x - v.x) < Math::epsilon && std::abs(y - v.y) < Math::epsilon && std::abs(z - v.z) < Math::epsilon) {
			return true;
		}
		return false;
	}

	inline bool operator!=(const Vec3& v) const {
		return !(*this == v);
	}

	inline Vec3 operator-() const {
		return Vec3(-x, -y, -z);
	}


	inline double length() const {
		return std::sqrt(x * x + y * y + z * z);
	}

	inline void normalize() {
		div(length());
	}

	inline Vec3 cross(const Vec3& v) const {
		return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	inline double dot(const Vec3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

	inline double distance(const Vec3& v) const {
		return Vec3(x - v.x, y - v.y, z - v.z).length();
	}

	inline double angle(const Vec3& v) const {
		arc_assert((length() > Math::epsilon) && (v.length() > Math::epsilon), "Vec3 angle with null vector");
		return std::acos(dot(v) / (length() * v.length()));
	}


	double x, y, z;

};


class Vec4 {

public:

	inline constexpr Vec4() : x(0), y(0), z(0), w(0) {}
	inline constexpr explicit Vec4(double value) : x(value), y(value), z(value), w(value) {}
	inline constexpr explicit Vec4(const Vec2& v) : x(v.x), y(v.y), z(0), w(0) {}
	inline constexpr explicit Vec4(const Vec3& v) : x(v.x), y(v.y), z(v.z), w(0) {}
	inline constexpr Vec4(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}


	inline void add(const Vec4& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}

	inline void sub(const Vec4& v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;
		w -= v.w;
	}

	inline void mul(double s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	inline void div(double s) {
		arc_assert(s != 0.0, "Vec4 divided by 0");
		x /= s;
		y /= s;
		z /= s;
		w /= s;
	}


	inline Vec4& operator=(const Vec4& v) {
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	inline Vec4& operator+=(const Vec4& v) {
		add(v);
		return *this;
	}

	inline Vec4& operator-=(const Vec4& v) {
		sub(v);
		return *this;
	}

	inline Vec4& operator*=(double s) {
		mul(s);
		return *this;
	}

	inline Vec4& operator/=(double s) {
		div(s);
		return *this;
	}

	inline bool operator==(const Vec4& v) const {
		if (std::abs(x - v.x) < Math::epsilon && std::abs(y - v.y) < Math::epsilon && std::abs(z - v.z) < Math::epsilon && std::abs(w - v.w) < Math::epsilon) {
			return true;
		}
		return false;
	}

	inline bool operator!=(const Vec4& v) const {
		return !(*this == v);
	}

	inline Vec4 operator-() const {
		return Vec4(-x, -y, -z, -w);
	}


	inline double length() const {
		return std::sqrt(x * x + y * y + z * z + w * w);
	}

	inline void normalize() {
		div(length());
	}

	inline double dot(const Vec4& v) const {
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}

	inline double distance(const Vec4& v) const {
		return Vec4(x - v.x, y - v.y, z - v.z, w - v.w).length();
	}

	inline double angle(const Vec4& v) const {
		arc_assert((length() > Math::epsilon) && (v.length() > Math::epsilon), "Vec4 angle with null vector");
		return std::acos(dot(v) / (length() * v.length()));
	}


	double x, y, z, w;

};


inline Vec2 operator+(Vec2 a, const Vec2& b) {
	a += b;
	return a;
}

inline Vec2 operator-(Vec2 a, const Vec2& b) {
	a -= b;
	return a;
}

inline Vec2 operator*(Vec2 v, double s) {
	v *= s;
	return v;
}

inline Vec2 operator/(Vec2 v, double s) {
	v /= s;
	return v;
}


inline Vec3 operator+(Vec3 a, const Vec3& b) {
	a += b;
	return a;
}

inline Vec3 operator-(Vec3 a, const Vec3& b) {
	a -= b;
	return a;
}

inline Vec3 operator*(Vec3 v, double s) {
	v *= s;
	return v;
}

inline Vec3 operator/(Vec3 v, double s) {
	v /= s;
	return v;
}


inline Vec4 operator+(Vec4 a, const Vec4& b) {
	a += b;
	return a;
}

inline Vec4 operator-(Vec4 a, const Vec4& b) {
	a -= b;
	return a;
}

inline Vec4 operator*(Vec4 v, double s) {
	v *= s;
	return v;
}

inline Vec4 operator/(Vec4 v, double s) {
	v /= s;
	return v;
}
