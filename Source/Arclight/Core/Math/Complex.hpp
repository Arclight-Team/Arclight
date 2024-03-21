/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Complex.hpp
 */

#pragma once

#include "Math.hpp"

#include <complex>



template<CC::Float T>
class Complex;


namespace CC {

	template<class T>
	concept Complex = CC::BaseOf<T, Complex<typename T::Type>>;

}


namespace TT {

	template<CC::Complex C, CC::Complex D>
	using CommonComplexType = Complex<CommonType<typename C::Type, typename D::Type>>;

}


template<CC::Float T>
class Complex {

public:

	using Type = T;


	template<CC::Float F>
	constexpr Complex(const Complex<F>& other) noexcept : imag(other.imag), real(other.real) {}

	template<CC::Arithmetic A = T, CC::Arithmetic B = A>
	constexpr Complex(A real = {}, B imag = {}) noexcept : imag(imag), real(real) {}


	template<CC::Arithmetic A>
	constexpr bool operator==(A value) const noexcept {
		return real == value;
	}

	template<CC::Float F>
	constexpr bool operator==(const Complex<F>& value) const noexcept {
		return (imag == value.imag) && (real == value.real);
	}


	template<CC::Arithmetic A>
	constexpr Complex& operator+=(A value) noexcept {
		real += value;
		return *this;
	}

	template<CC::Arithmetic A>
	constexpr Complex& operator-=(A value) noexcept {
		real -= value;
		return *this;
	}

	template<CC::Arithmetic A>
	constexpr Complex& operator*=(A value) noexcept {
		real *= value;
		imag *= value;
		return *this;
	}

	template<CC::Arithmetic A>
	constexpr Complex& operator/=(A value) noexcept {
		real /= value;
		imag /= value;
		return *this;
	}


	template<CC::Float F>
	constexpr Complex& operator+=(const Complex<F>& value) noexcept {
		imag += value.imag;
		real += value.real;
		return *this;
	}

	template<CC::Float F>
	constexpr Complex& operator-=(const Complex<F>& value) noexcept {
		imag -= value.imag;
		real -= value.real;
		return *this;
	}

	template<CC::Float F>
	constexpr Complex& operator*=(const Complex<F>& value) noexcept {

		T c = T(value.real);
		T d = T(value.imag);

		real = (real * c) - (imag * c);
		imag = (real * d) + (imag * d);

		// TODO C draft conforming special values

		return *this;

	}

	template<CC::Float F>
	constexpr Complex& operator/=(const Complex<F>& value) noexcept {

		if (value.imag == 0) {

			real /= T(value.real);
			imag /= T(value.real);

		} else if constexpr (TT::HasBiggerType<T>) {

			using B = TT::BiggerType<T>;

			B c = B(value.real);
			B d = B(value.imag);

			B denom = c * c + d * d;

			real = (real * c + imag * d) / denom;
			imag = (imag * c + real * d) / denom;

		} else { // Smith's method

			T c = T(value.real);
			T d = T(value.imag);

			if (Math::abs(c) >= Math::abs(d)) {

				T factor = d * (1 / c);
				T denom = d * factor + c;

				T r = (real + imag * factor) / denom;
				imag = (imag - real * factor) / denom;
				real = r;

			} else {

				T factor = c * (1 / d);
				T denom = c * factor + d;

				T r = (real * factor + imag) / denom;
				imag = (imag * factor - real) / denom;
				real = r;

			}

		}

		// TODO C draft conforming special values

		return *this;

	}


	constexpr Complex operator+() const noexcept {
		return *this;
	}

	constexpr Complex operator-() const noexcept {
		return {-real, -imag};
	}


	constexpr T magSquared() const noexcept {
		return real * real + imag * imag;
	}

	constexpr Complex conj() const noexcept {
		return {real, -imag};
	}

	constexpr Complex proj() const noexcept {

		if (Math::isInfinity(real) || Math::isInfinity(imag)) {
			return {Math::Traits<T>::Infinity, Math::copySign(0, imag)};
		}

		return *this;

	}

	ARC_CONSTEXPR_CMATH26 T arg() const noexcept {
		return Math::atan2(imag, real);
	}

	ARC_CONSTEXPR_CMATH26 T norm() const noexcept {
		return Math::sqrt(magSquared());
	}


	// TODO Use C++23 deducing-this

	template<SizeT I>
	auto&& get() & noexcept {
		return getter<I>(*this);
	}

	template<SizeT I>
	auto&& get() && noexcept {
		return getter<I>(*this);
	}

	template<SizeT I>
	auto&& get() const & noexcept {
		return getter<I>(*this);
	}

	template<SizeT I>
	auto&& get() const && noexcept {
		return getter<I>(*this);
	}


	T real;
	T imag;

private:

	template<SizeT I, class S>
	static auto&& getter(S&& self) noexcept {

		if constexpr (I == 0) {
			return std::forward<S>(self).real;
		} else {
			return std::forward<S>(self).imag;
		}

	}

};

template<CC::Arithmetic A, CC::Arithmetic B>
Complex(A, B) -> Complex<TT::ToFloat<TT::CommonType<A, B>>>;


template<CC::Complex C, CC::Complex D>
constexpr TT::CommonComplexType<C, D> operator+(C a, const D& b) noexcept {

	TT::CommonComplexType<C, D> ax = a;
	return ax += b;

}

template<CC::Complex C, CC::Complex D>
constexpr TT::CommonComplexType<C, D> operator-(C a, const D& b) noexcept {

	TT::CommonComplexType<C, D> ax = a;
	return ax -= b;

}

template<CC::Complex C, CC::Complex D>
constexpr TT::CommonComplexType<C, D> operator*(C a, const D& b) noexcept {

	TT::CommonComplexType<C, D> ax = a;
	return ax *= b;

}

template<CC::Complex C, CC::Complex D>
constexpr TT::CommonComplexType<C, D> operator/(C a, const D& b) noexcept {

	TT::CommonComplexType<C, D> ax = a;
	return ax /= b;

}


template<CC::Complex C, CC::Arithmetic A>
constexpr auto operator+(const C& a, A b) noexcept {

	Complex<TT::CommonType<typename C::Type, A>> ax = a;
	return ax += b;

}

template<CC::Complex C, CC::Arithmetic A>
constexpr auto operator-(const C& a, A b) noexcept {

	Complex<TT::CommonType<typename C::Type, A>> ax = a;
	return ax -= b;

}

template<CC::Complex C, CC::Arithmetic A>
constexpr auto operator*(const C& a, A b) noexcept {

	Complex<TT::CommonType<typename C::Type, A>> ax = a;
	return ax *= b;

}

template<CC::Complex C, CC::Arithmetic A>
constexpr auto operator/(const C& a, A b) noexcept {

	Complex<TT::CommonType<typename C::Type, A>> ax = a;
	return ax /= b;

}


template<CC::Complex C, CC::Arithmetic A>
constexpr auto operator+(A a, const C& b) noexcept {

	Complex<TT::CommonType<typename C::Type, A>> ax = a;
	return ax += b;

}

template<CC::Complex C, CC::Arithmetic A>
constexpr auto operator-(A a, const C& b) noexcept {

	Complex<TT::CommonType<typename C::Type, A>> ax = a;
	return ax -= b;

}

template<CC::Complex C, CC::Arithmetic A>
constexpr auto operator*(A a, const C& b) noexcept {

	Complex<TT::CommonType<typename C::Type, A>> ax = a;
	return ax *= b;

}

template<CC::Complex C, CC::Arithmetic A>
constexpr auto operator/(A a, const C& b) noexcept {

	Complex<TT::CommonType<typename C::Type, A>> ax = a;
	return ax /= b;

}


namespace Math {

	// Conforming to C draft's Annex G point 3

	template<CC::Float F>
	constexpr bool isFinite(const Complex<F>& value) noexcept {
		return isFinite(value.real) && isFinite(value.imag);
	}

	template<CC::Float F>
	constexpr bool isInfinity(const Complex<F>& value) noexcept {
		return isInfinity(value.real) || isInfinity(value.imag);
	}

	template<CC::Float F>
	constexpr bool isNaN(const Complex<F>& value) noexcept {
		return !isFinite(value) && !isInfinity(value);
	}

	template<CC::Float F>
	constexpr bool isZero(const Complex<F>& value) noexcept {
		return isZero(value.real) && isZero(value.imag);
	}


	template<CC::Arithmetic A>
	constexpr A real(A value) noexcept {
		return value;
	}

	template<CC::Float F>
	constexpr F real(const Complex<F>& value) noexcept {
		return value.real;
	}


	template<CC::Arithmetic A>
	constexpr A imag(A value) noexcept {
		return 0;
	}

	template<CC::Float F>
	constexpr F imag(const Complex<F>& value) noexcept {
		return value.imag;
	}


	template<CC::Float F>
	ARC_CONSTEXPR_CMATH26 F abs(const Complex<F>& value) noexcept {
		return value.norm();
	}


	template<CC::Arithmetic A, CC::Arithmetic B = A>
	ARC_CONSTEXPR_CMATH26 Complex<TT::ToFloat<TT::CommonType<A, B>>> polar(A r, B theta = {}) {

		arc_assert(r >= 0, "Negative polar distance");

		return {r * cos(theta), r * sin(theta)};

	}


	// TODO C draft conforming functions

}


template<CC::Float F>
struct std::tuple_size<Complex<F>> : integral_constant<SizeT, 2> {};

template<SizeT I, CC::Float F> requires (I < 2)
struct std::tuple_element<I, Complex<F>> {
	using type = F;
};


ARC_DEFINE_FLOAT_ALIASES(1, Complex, Complex)


namespace Math::inline Literals {

	constexpr Complex<float> operator""_if(long double value) noexcept {
		return {value};
	}

	constexpr Complex<double> operator""_i(long double value) noexcept {
		return {value};
	}

	constexpr Complex<float> operator""_il(long double value) noexcept {
		return {value};
	}

}
