/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Matrix.hpp
 */

#pragma once

#include "Math/Math.hpp"
#include "Math/Vector.hpp"
#include "Common/Intrinsic.hpp"

#include ARC_INTRINSIC_H



template<CC::Arithmetic T>
class Mat2;

template<CC::Arithmetic T>
class Mat3;

template<CC::Arithmetic T>
class Mat4;


namespace CC {

	template<class T>
	concept Matrix = CC::AnyOf<T, Mat2<typename T::Type>, Mat3<typename T::Type>, Mat4<typename T::Type>>;

	template<class T>
	concept FloatMatrix = Matrix<T> && CC::Float<typename T::Type>;

	template<class T>
	concept IntegerMatrix = Matrix<T> && CC::Integer<typename T::Type>;

	template<class T>
	concept IntegralMatrix = Matrix<T> && CC::Integral<typename T::Type>;

	template<class T, class U>
	concept EqualMatrixRank = Matrix<T> && Matrix<U> && T::Size == U::Size;

}

namespace TT {

	namespace Detail {

		template<CC::Matrix V, CC::Arithmetic T>
		struct SizedMatrix {
			using Type = TT::Conditional<V::Size == 2, Mat2<T>, TT::Conditional<V::Size == 3, Mat3<T>, Mat4<T>>>;
		};

	}

	template<CC::Matrix V, CC::Arithmetic T>
	using SizedMatrix = typename Detail::SizedMatrix<V, T>::Type;

	template<CC::Matrix A, CC::Matrix B> requires CC::EqualMatrixRank<A, B>
	using CommonMatrixType = SizedMatrix<A, TT::CommonType<typename A::Type, typename B::Type>>;

}


template<CC::Arithmetic T>
class Mat2 {

private:

	using X = TT::PromotedType<T>;

public:

	using Type = T;

	using VecT = Vec2<Type>;
	
	constexpr static SizeT Size = 2;


	constexpr Mat2() noexcept {
		setIdentity();
	}

	template<CC::Arithmetic A>
	constexpr Mat2(const Mat2<A>& m) noexcept : v{m[0], m[1]} {}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C, CC::Arithmetic D>
	constexpr Mat2(A a, B b, C c, D d) noexcept : v{{a, c}, {b, d}} {}

	constexpr Mat2(const Vec2<T>& a, const Vec2<T>& b) noexcept : v{a, b} {}


	template<CC::Arithmetic A>
	constexpr Mat2& add(const Mat2<A>& m) noexcept {

		v[0] += m[0];
		v[1] += m[1];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat2& subtract(const Mat2<A>& m) noexcept {

		v[0] -= m[0];
		v[1] -= m[1];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat2& multiply(const Mat2<A>& m) noexcept {

		T a = v[0][0] * m[0][0] + v[1][0] * m[0][1];
		T b = v[0][1] * m[0][0] + v[1][1] * m[0][1];
		T c = v[0][0] * m[1][0] + v[1][0] * m[1][1];
		T d = v[0][1] * m[1][0] + v[1][1] * m[1][1];

		return *this = Mat2(a, c, b, d);

	}

	template<CC::Arithmetic A>
	constexpr Mat2& multiply(A a) noexcept {

		v[0] *= a;
		v[1] *= a;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat2& divide(A a) noexcept {

		arc_assert(!Math::isZero(a), "Mat2 divided by 0");

		v[0] /= a;
		v[1] /= a;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat2& compMultiply(const Mat2<A>& m) noexcept {

		v[0].compMultiply(m[0]);
		v[1].compMultiply(m[1]);

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat2& compDivide(const Mat2<A>& m) noexcept {

		v[0].compDivide(m[0]);
		v[1].compDivide(m[1]);

		return *this;

	}
	

	constexpr void transpose() noexcept {
		std::swap(v[0][1], v[1][0]);
	}

	constexpr Mat2 transposed() const noexcept {

		Mat2 ret = *this;
		ret.transpose();

		return ret;

	}

	constexpr void invert() noexcept requires (CC::Float<T>) {

		T det = determinant();

		arc_assert(!Math::isZero(det), "Cannot invert matrix with determinant zero");

		T s = 1.0 / det;

		T t = v[0][0] * s;
		v[0][0] = v[1][1] * s;
		v[0][1] = -v[0][1] * s;
		v[1][0] = -v[1][0] * s;
		v[1][1] = t;

	}

	template<CC::Float F = TT::ToFloat<T>>
	constexpr Mat2<F> inverse() const noexcept {

		Mat2<F> ret = *this;
		ret.invert();

		return ret;

	}

	constexpr X determinant() const noexcept {
		return v[0][0] * v[1][1] - v[1][0] * v[0][1];
	}

	constexpr X trace() const noexcept {
		return v[0][0] + v[1][1];
	}

	constexpr void setIdentity() noexcept {

		v[0] = {1, 0};
		v[1] = {0, 1};

	}
	

	template<CC::Arithmetic A>
	constexpr Mat2& operator=(const Mat2<A>& m) noexcept {

		v[0] = m[0];
		v[1] = m[1];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat2& operator+=(const Mat2<A>& m) noexcept {
		return add(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat2& operator-=(const Mat2<A>& m) noexcept {
		return subtract(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat2& operator*=(const Mat2<A>& m) noexcept {
		return multiply(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat2& operator*=(A a) noexcept {
		return multiply(a);
	}

	template<CC::Arithmetic A>
	constexpr Mat2& operator/=(A a) noexcept {
		return divide(a);
	}
	

	template<CC::Arithmetic A>
	constexpr bool operator==(const Mat2<A>& m) const noexcept {
		return v[0] == m[0] && v[1] == m[1];
	}


	constexpr Mat2 operator+() const noexcept {
		return *this;
	}

	constexpr Mat2 operator-() const noexcept {
		return {-v[0], -v[1]};
	}


	constexpr VecT& operator[](SizeT index) noexcept {
		arc_assert(index < 2, "Mat2 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr const VecT& operator[](SizeT index) const noexcept {
		arc_assert(index < 2, "Mat2 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr T& operator()(SizeT row, SizeT column) noexcept {
		return *this[row][column];
	}

	constexpr const T& operator()(SizeT row, SizeT column) const noexcept {
		return *this[row][column];
	}


	VecT v[2];

};


template<CC::Arithmetic T>
class Mat3 {

private:

	using X = TT::PromotedType<T>;

public:

	using Type = T;

	using VecT = Vec3<Type>;

	constexpr static SizeT Size = 3;


	constexpr Mat3() noexcept {
		setIdentity();
	}

	template<CC::Arithmetic A>
	constexpr Mat3(const Mat3<A>& m) noexcept : v{m[0], m[1], m[2]} {}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C, CC::Arithmetic D, CC::Arithmetic E, CC::Arithmetic F, CC::Arithmetic G, CC::Arithmetic H, CC::Arithmetic I>
	constexpr Mat3(A a, B b, C c, D d, E e, F f, G g, H h, I i) noexcept : v{{a, d, g}, {b, e, h}, {c, f, i}} {}

	constexpr Mat3(const VecT& a, const VecT& b, const VecT& c) noexcept : v{a, b, c} {}

	constexpr explicit Mat3(const Mat2<T>& m) noexcept : v{{m[0][0], m[0][1], 0}, {m[1][0], m[1][1], 0}, {0, 0, 1}} {}


	template<CC::Arithmetic A>
	constexpr Mat3& add(const Mat3<A>& m) noexcept {

		v[0] += m[0];
		v[1] += m[1];
		v[2] += m[2];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat3& subtract(const Mat3<A>& m) noexcept {

		v[0] -= m[0];
		v[1] -= m[1];
		v[2] -= m[2];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat3& multiply(const Mat3<A>& m) noexcept {

		T a = v[0][0] * m[0][0] + v[1][0] * m[0][1] + v[2][0] * m[0][2];
		T b = v[0][1] * m[0][0] + v[1][1] * m[0][1] + v[2][1] * m[0][2];
		T c = v[0][2] * m[0][0] + v[1][2] * m[0][1] + v[2][2] * m[0][2];
		T d = v[0][0] * m[1][0] + v[1][0] * m[1][1] + v[2][0] * m[1][2];
		T e = v[0][1] * m[1][0] + v[1][1] * m[1][1] + v[2][1] * m[1][2];
		T f = v[0][2] * m[1][0] + v[1][2] * m[1][1] + v[2][2] * m[1][2];
		T g = v[0][0] * m[2][0] + v[1][0] * m[2][1] + v[2][0] * m[2][2];
		T h = v[0][1] * m[2][0] + v[1][1] * m[2][1] + v[2][1] * m[2][2];
		T i = v[0][2] * m[2][0] + v[1][2] * m[2][1] + v[2][2] * m[2][2];

		return *this = Mat3(a, d, h, b, e, h, c, f, i);

	}

	template<CC::Arithmetic A>
	constexpr Mat3& multiply(A a) noexcept {

		v[0] *= a;
		v[1] *= a;
		v[2] *= a;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat3& divide(A a) noexcept {

		arc_assert(!Math::isZero(a), "Mat3 divided by 0");

		v[0] /= a;
		v[1] /= a;
		v[2] /= a;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat3& compMultiply(const Mat3<A>& m) noexcept {

		v[0].compMultiply(m[0]);
		v[1].compMultiply(m[1]);
		v[2].compMultiply(m[2]);

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat3& compDivide(const Mat3<A>& m) noexcept {

		v[0].compDivide(m[0]);
		v[1].compDivide(m[1]);
		v[2].compDivide(m[2]);

		return *this;

	}


	constexpr void transpose() noexcept {

		std::swap(v[0][1], v[1][0]);
		std::swap(v[0][2], v[2][0]);
		std::swap(v[1][2], v[2][1]);

	}

	constexpr Mat3 transposed() const noexcept {

		Mat3 ret = *this;
		ret.transpose();

		return ret;

	}

	constexpr void invert() noexcept requires (CC::Float<T>) {

		T det = determinant();
		
		arc_assert(!Math::isZero(det), "Mat3 inverse failed: Matrix not invertible");

		T s = 1 / det;

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

	template<CC::Float F = TT::ToFloat<T>>
	constexpr Mat3<F> inverse() const noexcept {

		Mat3<F> ret = *this;
		ret.invert();

		return ret;

	}

	constexpr X determinant() const noexcept {
		return v[0][0] * v[1][1] * v[2][2] + v[1][0] * v[2][1] * v[0][2] + v[2][0] * v[0][1] * v[1][2]
			   - v[0][2] * v[1][1] * v[2][0] - v[1][2] * v[2][1] * v[0][0] - v[2][2] * v[0][1] * v[1][0];
	}

	constexpr X trace() const noexcept {
		return v[0][0] + v[1][1] + v[2][2];
	}

	constexpr void setIdentity() noexcept {

		v[0] = {1, 0, 0};
		v[1] = {0, 1, 0};
		v[2] = {0, 0, 1};

	}


	template<CC::Arithmetic A>
	constexpr Mat3& operator=(const Mat3<A>& m) noexcept {

		v[0] = m[0];
		v[1] = m[1];
		v[2] = m[2];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat3& operator+=(const Mat3<A>& m) noexcept {
		return add(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat3& operator-=(const Mat3<A>& m) noexcept {
		return subtract(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat3& operator*=(const Mat3<A>& m) noexcept {
		return multiply(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat3& operator*=(A a) noexcept {
		return multiply(a);
	}

	template<CC::Arithmetic A>
	constexpr Mat3& operator/=(A a) noexcept {
		return divide(a);
	}
	

	template<CC::Arithmetic A>
	constexpr bool operator==(const Mat3<A>& m) const noexcept {
		return v[0] == m[0] && v[1] == m[1] && v[2] == m[2];
	}


	constexpr Mat3 operator+() const noexcept {
		return *this;
	}

	constexpr Mat3 operator-() const noexcept {
		return {-v[0], -v[1], -v[2]};
	}
	

	constexpr VecT& operator[](SizeT index) noexcept {
		arc_assert(index < 3, "Mat3 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr const VecT& operator[](SizeT index) const noexcept {
		arc_assert(index < 3, "Mat3 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr T& operator()(SizeT row, SizeT column) noexcept {
		return *this[row][column];
	}

	constexpr const T& operator()(SizeT row, SizeT column) const noexcept {
		return *this[row][column];
	}


	template<CC::Arithmetic A, CC::Arithmetic B>
	constexpr Mat3& translate(A x, B y) noexcept {

		v[2] += v[0] * x;
		v[2] += v[1] * y;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat3& translate(const Vec2<A>& vec) noexcept {
		return translate(vec.x, vec.y);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 Mat3& rotate(A angle) {

		const auto [sin, cos] = Math::sincos(angle);

		VecT x = v[0] * cos + v[1] * sin;
		VecT y = v[0] * -sin + v[1] * cos;
		v[0] = x;
		v[1] = y;

		return *this;

	}

	template<CC::Arithmetic A, CC::Arithmetic B>
	constexpr Mat3& scale(A x, B y) noexcept {

		v[0] *= x;
		v[1] *= y;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat3& scale(A v) noexcept {
		return scale(v, v);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 Mat3& shearX(A angle) {

		v[1] += v[0] * Math::tan(angle);
		return *this;

	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 Mat3& shearY(A angle) {

		v[0] += v[1] * Math::tan(angle);
		return *this;

	}

	template<CC::Arithmetic A, CC::Arithmetic B>
	ARC_CONSTEXPR_CMATH26 Mat3& shear(A sx, B sy) {

		A tx = Math::tan(sx);
		B ty = Math::tan(sy);
		auto tn = tx * ty + 1;

		VecT x = v[0] * tn + v[1] * ty;
		VecT y = v[0] * tx + v[1];

		v[0] = x;
		v[1] = y;

		return *this;

	}


	constexpr static Mat3 fromTranslation(T x, T y) noexcept {
		return {1, 0, x, 0, 1, y, 0, 0, 1};
	}

	constexpr static Mat3 fromTranslation(const VecT& vec) noexcept {
		return fromTranslation(vec.x, vec.y);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 static Mat3 fromRotation(A angle) {

		const auto [sin, cos] = Math::sincos(angle);

		return {cos, -sin, 0, sin, cos, 0, 0, 0, 1};

	}

	constexpr static Mat3 fromScale(T x, T y) noexcept {
		return {x, 0, 0, 0, y, 0, 0, 0, 1};
	}

	template<CC::Arithmetic A>
	constexpr static Mat3 fromScale(T v) noexcept {
		return fromScale(v, v);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 static Mat3 fromShearX(A angle) {
		return {1, Math::tan(angle), 0, 0, 1, 0, 0, 0, 1};
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 static Mat3 fromShearY(A angle) {
		return {1, 0, 0, Math::tan(angle), 1, 0, 0, 0, 1};
	}

	template<CC::Arithmetic A, CC::Arithmetic B>
	ARC_CONSTEXPR_CMATH26 static Mat3 fromShear(A sx, B sy) {

		A tx = Math::tan(sx);
		B ty = Math::tan(sy);

		return Mat3(tx * ty + 1, tx, 0, ty, 1, 0, 0, 0, 1);

	}


	constexpr Mat2<T> toMat2(SizeT r = 0, SizeT c = 0) const noexcept {

		arc_assert(r >= 0 && r <= 1, "Matrix conversion row offset %d out of bounds", r);
		arc_assert(c >= 0 && c <= 1, "Matrix conversion column offset %d out of bounds", c);

		return {
			v[r + 0][c + 0], v[r + 1][c + 0],
			v[r + 0][c + 1], v[r + 1][c + 1]
		};

	}


	VecT v[3];

};


template<CC::Arithmetic T>
class alignas(16) Mat4 {

	using X = TT::PromotedType<T>;
	using FX = TT::ToFloat<T>;

public:

	using Type = T;

	using VecT = Vec4<Type>;
	
	constexpr static SizeT Size = 4;


	constexpr Mat4() {
		setIdentity();
	}

	template<CC::Arithmetic A>
	constexpr Mat4(const Mat4<A>& m) noexcept : v{m[0], m[1], m[2], m[3]} {}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C, CC::Arithmetic D,
			 CC::Arithmetic E, CC::Arithmetic F, CC::Arithmetic G, CC::Arithmetic H,
			 CC::Arithmetic I, CC::Arithmetic J, CC::Arithmetic K, CC::Arithmetic L,
			 CC::Arithmetic M, CC::Arithmetic N, CC::Arithmetic O, CC::Arithmetic P>
	constexpr Mat4(A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n, O o, P p) noexcept
		: v{{a, e, i, m}, {b, f, j, n}, {c, g, k, o}, {d, h, l, p}} {}

	constexpr Mat4(const VecT& a, const VecT& b, const VecT& c, const VecT d) noexcept : v{a, b, c, d} {}

	constexpr explicit Mat4(const Mat2<T>& m) noexcept
		: v{{m[0][0], m[0][1], 0, 0}, {m[1][0], m[1][1], 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}} {}

	constexpr explicit Mat4(const Mat3<T>& m) noexcept
		: v{{m[0][0], m[0][1], m[0][2], 0}, {m[1][0], m[1][1], m[1][2], 0}, {m[2][0], m[2][1], m[2][2], 0}, {0, 0, 0, 1}} {}


	template<CC::Arithmetic A>
	constexpr Mat4& add(const Mat4<A>& m) noexcept {

		v[0] += m[0];
		v[1] += m[1];
		v[2] += m[2];
		v[3] += m[3];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat4& subtract(const Mat4<A>& m) noexcept {

		v[0] -= m[0];
		v[1] -= m[1];
		v[2] -= m[2];
		v[3] -= m[3];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat4& multiply(const Mat4<A>& t) noexcept {

		arc_intrinsic_sse (

			if constexpr (CC::Equal<TT::CommonType<T, A>, float>) {

				__m128 l0 = _mm_set_ps(v[0][3], v[0][2], v[0][1], v[0][0]);
				__m128 l1 = _mm_set_ps(v[1][3], v[1][2], v[1][1], v[1][0]);
				__m128 l2 = _mm_set_ps(v[2][3], v[2][2], v[2][1], v[2][0]);
				__m128 l3 = _mm_set_ps(v[3][3], v[3][2], v[3][1], v[3][0]);

				__m128 r0 = _mm_set_ps(t[0][3], t[0][2], t[0][1], t[0][0]);

				__m128 x00 = _mm_mul_ps(l0, _mm_shuffle_ps(r0, r0, 0x00));
				__m128 x01 = _mm_mul_ps(l1, _mm_shuffle_ps(r0, r0, 0x55));
				__m128 x02 = _mm_mul_ps(l2, _mm_shuffle_ps(r0, r0, 0xAA));
				__m128 x03 = _mm_mul_ps(l3, _mm_shuffle_ps(r0, r0, 0xFF));

				__m128 c0 = _mm_add_ps(_mm_add_ps(x00, x01), _mm_add_ps(x02, x03));

				__m128 r1 = _mm_set_ps(t[1][3], t[1][2], t[1][1], t[1][0]);

				__m128 x10 = _mm_mul_ps(l0, _mm_shuffle_ps(r1, r1, 0x00));
				__m128 x11 = _mm_mul_ps(l1, _mm_shuffle_ps(r1, r1, 0x55));
				__m128 x12 = _mm_mul_ps(l2, _mm_shuffle_ps(r1, r1, 0xAA));
				__m128 x13 = _mm_mul_ps(l3, _mm_shuffle_ps(r1, r1, 0xFF));

				__m128 c1 = _mm_add_ps(_mm_add_ps(x10, x11), _mm_add_ps(x12, x13));

				__m128 r2 = _mm_set_ps(t[2][3], t[2][2], t[2][1], t[2][0]);

				__m128 x20 = _mm_mul_ps(l0, _mm_shuffle_ps(r2, r2, 0x00));
				__m128 x21 = _mm_mul_ps(l1, _mm_shuffle_ps(r2, r2, 0x55));
				__m128 x22 = _mm_mul_ps(l2, _mm_shuffle_ps(r2, r2, 0xAA));
				__m128 x23 = _mm_mul_ps(l3, _mm_shuffle_ps(r2, r2, 0xFF));

				__m128 c2 = _mm_add_ps(_mm_add_ps(x20, x21), _mm_add_ps(x22, x23));

				__m128 r3 = _mm_set_ps(t[3][3], t[3][2], t[3][1], t[3][0]);

				__m128 x30 = _mm_mul_ps(l0, _mm_shuffle_ps(r3, r3, 0x00));
				__m128 x31 = _mm_mul_ps(l1, _mm_shuffle_ps(r3, r3, 0x55));
				__m128 x32 = _mm_mul_ps(l2, _mm_shuffle_ps(r3, r3, 0xAA));
				__m128 x33 = _mm_mul_ps(l3, _mm_shuffle_ps(r3, r3, 0xFF));

				__m128 c3 = _mm_add_ps(_mm_add_ps(x30, x31), _mm_add_ps(x32, x33));


				alignas(16) float f[4];

				_mm_store_ps(f, c0);

				v[0].x = f[0];
				v[0].y = f[1];
				v[0].z = f[2];
				v[0].w = f[3];

				_mm_store_ps(f, c1);

				v[1].x = f[0];
				v[1].y = f[1];
				v[1].z = f[2];
				v[1].w = f[3];

				_mm_store_ps(f, c2);

				v[2].x = f[0];
				v[2].y = f[1];
				v[2].z = f[2];
				v[2].w = f[3];

				_mm_store_ps(f, c3);

				v[3].x = f[0];
				v[3].y = f[1];
				v[3].z = f[2];
				v[3].w = f[3];

				return *this;

			}

		)

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

		return *this = Mat4(a, e, i, m, b, f, j, n, c, g, k, o, d, h, l, p);

	}

	template<CC::Arithmetic A>
	constexpr Mat4& multiply(A a) noexcept {

		v[0] *= a;
		v[1] *= a;
		v[2] *= a;
		v[3] *= a;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat4& divide(A a) noexcept {

		arc_assert(!Math::isZero(a), "Mat4 divided by 0");

		v[0] /= a;
		v[1] /= a;
		v[2] /= a;
		v[3] /= a;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat4& compMultiply(const Mat4<A>& m) noexcept {

		v[0].compMultiply(m[0]);
		v[1].compMultiply(m[1]);
		v[2].compMultiply(m[2]);
		v[3].compMultiply(m[3]);

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat4& compDivide(const Mat4<A>& m) noexcept {

		v[0].compDivide(m[0]);
		v[1].compDivide(m[1]);
		v[2].compDivide(m[2]);
		v[3].compDivide(m[3]);

		return *this;

	}
	

	constexpr void transpose() noexcept {

		std::swap(v[0][1], v[1][0]);
		std::swap(v[0][2], v[2][0]);
		std::swap(v[0][3], v[3][0]);
		std::swap(v[1][2], v[2][1]);
		std::swap(v[1][3], v[3][1]);
		std::swap(v[2][3], v[3][2]);

	}

	constexpr Mat4 transposed() const noexcept {

		Mat4 ret = *this;
		ret.transpose();

		return ret;

	}

	constexpr void invert() noexcept requires (CC::Float<T>) {

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

		v[0][0] = (f * k * p - f * l * o - j * g * p + j * h * o + n * g * l - n * h * k) * s;
		v[0][1] = (-e * k * p + e * l * o + i * g * p - i * h * o - m * g * l + m * h * k) * s;
		v[0][2] = (e * j * p - e * l * n - i * f * p + i * h * n + m * f * l - m * h * j) * s;
		v[0][3] = (-e * j * o + e * k * n + i * f * o - i * g * n - m * f * k + m * g * j) * s;
		v[1][0] = (-b * k * p + b * l * o + j * c * p - j * d * o - n * c * l + n * d * k) * s;
		v[1][1] = (a * k * p - a * l * o - i * c * p + i * d * o + m * c * l - m * d * k) * s;
		v[1][2] = (-a * j * p + a * l * n + i * b * p - i * d * n - m * b * l + m * d * j) * s;
		v[1][3] = (a * j * o - a * k * n - i * b * o + i * c * n + m * b * k - m * c * j) * s;
		v[2][0] = (b * g * p - b * h * o - f * c * p + f * d * o + n * c * h - n * d * g) * s;
		v[2][1] = (-a * g * p + a * h * o + e * c * p - e * d * o - m * c * h + m * d * g) * s;
		v[2][2] = (a * f * p - a * h * n - e * b * p + e * d * n + m * b * h - m * d * f) * s;
		v[2][3] = (-a * f * o + a * g * n + e * b * o - e * c * n - m * b * g + m * c * f) * s;
		v[3][0] = (-b * g * l + b * h * k + f * c * l - f * d * k - j * c * h + j * d * g) * s;
		v[3][1] = (a * g * l - a * h * k - e * c * l + e * d * k + i * c * h - i * d * g) * s;
		v[3][2] = (-a * f * l + a * h * j + e * b * l - e * d * j - i * b * h + i * d * f) * s;
		v[3][3] = (a * f * k - a * g * j - e * b * k + e * c * j + i * b * g - i * c * f) * s;

	}

	template<CC::Float F = TT::ToFloat<T>>
	constexpr Mat4<F> inverse() const noexcept {

		Mat4<F> ret = *this;
		ret.invert();

		return ret;

	}

	constexpr X determinant() const noexcept {

		X a = v[0][0] * (v[1][1] * v[2][2] * v[3][3] + v[2][1] * v[3][2] * v[1][3] + v[3][1] * v[1][2] * v[2][3]
						 - v[1][3] * v[2][2] * v[3][1] - v[2][3] * v[3][2] * v[1][1] - v[3][3] * v[1][2] * v[2][1]);
		X b = v[1][0] * (v[0][1] * v[2][2] * v[3][3] + v[2][1] * v[3][2] * v[0][3] + v[3][1] * v[0][2] * v[2][3]
						 - v[0][3] * v[2][2] * v[3][1] - v[2][3] * v[3][2] * v[0][1] - v[3][3] * v[0][2] * v[2][1]);
		X c = v[2][0] * (v[0][1] * v[1][2] * v[3][3] + v[1][1] * v[3][2] * v[0][3] + v[3][1] * v[0][2] * v[1][3]
						 - v[0][3] * v[1][2] * v[3][1] - v[1][3] * v[3][2] * v[0][1] - v[3][3] * v[0][2] * v[1][1]);
		X d = v[3][0] * (v[0][1] * v[1][2] * v[2][3] + v[1][1] * v[2][2] * v[0][3] + v[2][1] * v[0][2] * v[1][3]
						 - v[0][3] * v[1][2] * v[2][1] - v[1][3] * v[2][2] * v[0][1] - v[2][3] * v[0][2] * v[1][1]);

		return a - b + c - d;

	}

	constexpr X trace() const noexcept {
		return v[0][0] + v[1][1] + v[2][2] + v[3][3];
	}

	constexpr void setIdentity() noexcept {

		v[0] = {1, 0, 0, 0};
		v[1] = {0, 1, 0, 0};
		v[2] = {0, 0, 1, 0};
		v[3] = {0, 0, 0, 1};

	}
	

	template<CC::Arithmetic A>
	constexpr Mat4& operator=(const Mat4<A>& m) noexcept {

		v[0] = m[0];
		v[1] = m[1];
		v[2] = m[2];
		v[3] = m[3];

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat4& operator+=(const Mat4<A>& m) noexcept {
		return add(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat4& operator-=(const Mat4<A>& m) noexcept {
		return subtract(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat4& operator*=(const Mat4<A>& m) noexcept {
		return multiply(m);
	}

	template<CC::Arithmetic A>
	constexpr Mat4& operator*=(A a) noexcept {
		return multiply(a);
	}

	template<CC::Arithmetic A>
	constexpr Mat4& operator/=(A a) noexcept {
		return divide(a);
	}
	

	template<CC::Arithmetic A>
	constexpr bool operator==(const Mat4<A>& m) const noexcept {
		return v[0] == m[0] && v[1] == m[1] && v[2] == m[2] && v[3] == m[3];
	}


	constexpr Mat4 operator+() const noexcept {
		return *this;
	}

	constexpr Mat4 operator-() const noexcept {
		return {-v[0], -v[1], -v[2], -v[3]};
	}
	

	constexpr VecT& operator[](SizeT index) noexcept {
		arc_assert(index < 4, "Mat4 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr const VecT& operator[](SizeT index) const noexcept {
		arc_assert(index < 4, "Mat4 access out of bounds (index=%d)", index);
		return v[index];
	}

	constexpr T& operator()(SizeT row, SizeT column) noexcept {
		return *this[row][column];
	}

	constexpr const T& operator()(SizeT row, SizeT column) const noexcept {
		return *this[row][column];
	}


	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	constexpr Mat4& translate(A x, B y, C z) noexcept {

		v[3] += v[0] * x;
		v[3] += v[1] * y;
		v[3] += v[2] * z;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat4& translate(const Vec3<A>& vec) noexcept {
		return translate(vec.x, vec.y, vec.z);
	}

	template<CC::Arithmetic A>
	ARC_CONSTEXPR_CMATH26 Mat4& rotate(const Vec3<A>& axis, FX angle) {

		using Y = TT::CommonType<T, A>;

		Vec3<Y> u = Vec3<Y>::normalize(axis);

		const auto [sin, cos] = Math::sincos(angle);
		FX mcos = FX(1) - cos;

		Mat3<Y> n(u.x * u.x * mcos + cos, u.x * u.y * mcos - u.z * sin, u.x * u.z * mcos + u.y * sin,
				  u.x * u.y * mcos + u.z * sin, u.y * u.y * mcos + cos, u.y * u.z * mcos - u.x * sin,
				  u.x * u.z * mcos - u.y * sin, u.y * u.z * mcos + u.x * sin, u.z * u.z * mcos + cos);

		VecT x = v[0] * n[0][0] + v[1] * n[0][1] + v[2] * n[0][2];
		VecT y = v[0] * n[1][0] + v[1] * n[1][1] + v[2] * n[1][2];
		VecT z = v[0] * n[2][0] + v[1] * n[2][1] + v[2] * n[2][2];
		v[0] = x;
		v[1] = y;
		v[2] = z;

		return *this;

	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	constexpr Mat4& scale(A x, B y, C z) noexcept {

		v[0] *= x;
		v[1] *= y;
		v[2] *= z;

		return *this;

	}

	template<CC::Arithmetic A>
	constexpr Mat4& scale(A v) noexcept {
		return scale(v, v, v);
	}

	template<CC::Arithmetic A>
	constexpr Mat4& scale(const Vec3<A>& vec) noexcept {
		return scale(vec.x, vec.y, vec.z);
	}


	constexpr static Mat4 fromTranslation(T x, T y, T z) noexcept {
		return {1, 0, 0, x, 0, 1, 0, y, 0, 0, 1, z, 0, 0, 0, 1};
	}

	constexpr static Mat4 fromTranslation(const Vec3<T>& vec) noexcept {
		return fromTranslation(vec.x, vec.y, vec.z);
	}

	template<CC::Arithmetic A, CC::Arithmetic B>
	ARC_CONSTEXPR_CMATH26 static Mat4 fromRotation(const Vec3<A>& axis, B angle) {

		using Y = TT::CommonType<T, A>;

		Vec3<Y> u = Vec3<Y>::normalize(axis);
		
		const auto [sin, cos] = Math::sincos(angle);
		B mcos = B(1) - cos;

		return {
			u.x * u.x * mcos + cos, u.x * u.y * mcos - u.z * sin, u.x * u.z * mcos + u.y * sin, 0,
			u.x * u.y * mcos + u.z * sin, u.y * u.y * mcos + cos, u.y * u.z * mcos - u.x * sin, 0,
			u.x * u.z * mcos - u.y * sin, u.y * u.z * mcos + u.x * sin, u.z * u.z * mcos + cos, 0,
			0, 0, 0, 1
		};

	}

	template<CC::Arithmetic A, CC::Arithmetic B, CC::Arithmetic C>
	ARC_CONSTEXPR_CMATH26 static Mat4 fromRotation(A yaw, B pitch, C roll) {
		
		const auto [ysin, ycos] = Math::sincos(yaw);
		const auto [psin, pcos] = Math::sincos(pitch);
		const auto [rsin, rcos] = Math::sincos(roll);

		return {
			ycos * pcos, ycos * psin * rsin - ysin * rcos, ycos * psin * rcos + ysin * rsin, 0,
			ysin * pcos, ysin * psin * rsin + ycos * rcos, ysin * psin * rcos - ycos * rsin, 0,
			-psin, pcos * rsin, pcos * rcos, 0,
			0, 0, 0, 1
		};

	}

	constexpr static Mat4 fromScale(T x, T y, T z) noexcept {
		return {x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1};
	}

	constexpr static Mat4 fromScale(T v) noexcept {
		return fromScale(v, v, v);
	}

	constexpr static Mat4 fromScale(const Vec3<T>& vec) noexcept {
		return fromScale(vec.x, vec.y, vec.z);
	}


	template<CC::Arithmetic A, CC::Arithmetic B>
	ARC_CONSTEXPR_CMATH26 static Mat4 lookAt(const Vec3<A>& pos, const Vec3<B>& target, const Vec3<FX>& up = {0, 1, 0}) {

		using Y = TT::CommonType<T, A>;

		Vec3<Y> f = Vec3<Y>::normalize(pos - target);
		Vec3<Y> r = Vec3<Y>::normalize(up.cross(f));
		Vec3<Y> u = f.cross(r);

		return {
			r.x, r.y, r.z, -r.dot(pos),
			u.x, u.y, u.z, -u.dot(pos),
			f.x, f.y, f.z, -f.dot(pos),
			0, 0, 0, 1
		};

	}

	constexpr static Mat4 frustum(FX left, FX right, FX bottom, FX top, FX near, FX far) noexcept {

		return {
			2 * near / (right - left), 0, (right + left) / (right - left), 0,
			0, 2 * near / (top - bottom), (top + bottom) / (top - bottom), 0,
			0, 0, -(far + near) / (far - near), -2 * far * near / (far - near),
			0, 0, -1, 0
		};

	}

	ARC_CONSTEXPR_CMATH26 static Mat4 perspective(FX fovy, FX aspect, FX near, FX far) {

		FX t = Math::tan(fovy / FX(2));

		return {
			1 / (aspect * t), 0, 0, 0,
			0, 1 / t, 0, 0,
			0, 0, -(far + near) / (far - near), -2 * far * near / (far - near),
			0, 0, -1, 0
		};

	}

	constexpr static Mat4 ortho(FX left, FX right, FX bottom, FX top, FX near, FX far) noexcept {

		return {
			2 / (right - left), 0, 0, -(right + left) / (right - left),
			0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
			0, 0, -2 / (far - near), -(far + near) / (far - near),
			0, 0, 0, 1
		};

	}

	constexpr static Mat4 fromAffine2D(const Mat3<T>& m) noexcept {

		return {
			m[0][0], m[1][0], 0, m[2][0],
			m[0][1], m[1][1], 0, m[2][1],
			0, 0, 1, 0,
			m[0][2], m[1][2], 0, m[2][2]
		};

	}


	constexpr Mat2<T> toMat2(SizeT r = 0, SizeT c = 0) const noexcept {

		arc_assert(r >= 0 && r <= 2, "Matrix conversion row offset %d out of bounds", r);
		arc_assert(c >= 0 && c <= 2, "Matrix conversion column offset %d out of bounds", c);

		return {
			v[r + 0][c + 0], v[r + 1][c + 0],
			v[r + 0][c + 1], v[r + 1][c + 1]
		};

	}

	constexpr Mat3<T> toMat3(SizeT r = 0, SizeT c = 0) const noexcept {

		arc_assert(r >= 0 && r <= 1, "Matrix conversion row offset %d out of bounds", r);
		arc_assert(c >= 0 && c <= 1, "Matrix conversion column offset %d out of bounds", c);

		return {
			v[r + 0][c + 0], v[r + 1][c + 0], v[r + 2][c + 0],
			v[r + 0][c + 1], v[r + 1][c + 1], v[r + 2][c + 1],
			v[r + 0][c + 2], v[r + 1][c + 2], v[r + 2][c + 2]
		};

	}


	VecT v[4];

};


template<CC::Matrix A, CC::EqualMatrixRank<A> B>
constexpr TT::CommonMatrixType<A, B> operator+(A m, const B& n) noexcept {

	TT::CommonMatrixType<A, B> mx = m;
	return mx += n;

}

template<CC::Matrix A, CC::EqualMatrixRank<A> B>
constexpr TT::CommonMatrixType<A, B> operator-(A m, const B& n) noexcept {

	TT::CommonMatrixType<A, B> mx = m;
	return mx -= n;

}


template<CC::Matrix A, CC::EqualMatrixRank<A> B>
constexpr TT::CommonMatrixType<A, B> operator*(A m, const B& n) noexcept {

	TT::CommonMatrixType<A, B> mx = m;
	return mx *= n;

}

template<CC::Matrix A, CC::Arithmetic B>
constexpr auto operator*(A m, B b) noexcept {

	TT::SizedMatrix<A, TT::CommonType<typename A::Type, B>> mx = m;
	return mx *= b;

}

template<CC::Matrix A, CC::Arithmetic B>
constexpr auto operator*(B b, A m) noexcept {
	return m * b;
}


template<CC::Matrix A, CC::Arithmetic B>
constexpr auto operator/(A m, B b) noexcept {

	TT::SizedMatrix<A, TT::CommonType<typename A::Type, B>> mx = m;
	return mx /= b;

}


template<CC::Arithmetic A, CC::Arithmetic B>
constexpr Vec2<TT::CommonType<A, B>> operator*(const Mat2<A>& m, const Vec2<B>& v) noexcept {

	auto x = m[0][0] * v[0] + m[1][0] * v[1];
	auto y = m[0][1] * v[0] + m[1][1] * v[1];

	return { x, y };

}

template<CC::Arithmetic A, CC::Arithmetic B>
constexpr Vec3<TT::CommonType<A, B>> operator*(const Mat3<A>& m, const Vec3<B>& v) noexcept {

	auto x = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2];
	auto y = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2];
	auto z = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2];

	return { x, y, z };

}

template<CC::Arithmetic A, CC::Arithmetic B>
constexpr Vec4<TT::CommonType<A, B>> operator*(const Mat4<A>& m, const Vec4<B>& v) noexcept {

	auto x = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
	auto y = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
	auto z = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
	auto w = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];

	return { x, y, z, w };

}


template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A& operator&=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralMatrix<B>) {
			a[i] &= b[i];
		} else {
			a[i] &= b;
		}

	}

	return a;

}

template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A& operator|=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralMatrix<B>) {
			a[i] |= b[i];
		} else {
			a[i] |= b;
		}

	}

	return a;

}

template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A& operator^=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralMatrix<B>) {
			a[i] ^= b[i];
		} else {
			a[i] ^= b;
		}

	}

	return a;

}


template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A operator&(A a, const B& b) noexcept {
	return a &= b;
}

template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A operator|(A a, const B& b) noexcept {
	return a |= b;
}

template<CC::IntegralMatrix A, class B> requires(CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A operator^(A a, const B& b) noexcept {
	return a ^= b;
}

template<CC::IntegralMatrix A>
constexpr A operator~(A a) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {
		a[i] = ~a[i];
	}

	return a;

}


template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A& operator<<=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralMatrix<B>) {
			a[i] <<= b[i];
		} else {
			a[i] <<= b;
		}

	}

	return a;

}

template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A& operator>>=(A& a, const B& b) noexcept {

	for (SizeT i = 0; i < A::Size; i++) {

		if constexpr (CC::IntegralMatrix<B>) {
			a[i] >>= b[i];
		} else {
			a[i] >>= b;
		}

	}

	return a;

}

template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A operator<<(A a, const B& b) noexcept {
	return a <<= b;
}

template<CC::IntegralMatrix A, class B> requires (CC::Integral<B> || (CC::IntegralMatrix<B> && CC::EqualMatrixRank<A, B>))
constexpr A operator>>(A a, const B& b) noexcept {
	return a >>= b;
}


template<CC::Matrix M>
inline RawLog& operator<<(RawLog& log, const M& mat) {

	log << "Mat" << M::Size << "[";

	for (u32 i = 0; i < M::Size; i++) {

		log << "[";

		for (u32 j = 0; j < M::Size - 1; j++) {
			log << mat[j][i] <<  ", ";
		}

		log << mat[M::Size - 1][i];

		if (i != M::Size - 1) {
			log << "], ";
		} else {
			log << "]]";
		}

	}

	return log;

}


ARC_DEFINE_ARITHMETIC_ALIASES(0, Mat2, Mat2)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Mat3, Mat3)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Mat4, Mat4)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Mat2, Matrix2)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Mat3, Matrix3)
ARC_DEFINE_ARITHMETIC_ALIASES(0, Mat4, Matrix4)
