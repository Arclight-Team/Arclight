/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 FixedPoint.hpp
 */

#pragma once

#include "Math.hpp"
#include "BigInt.hpp"
#include "Overflow.hpp"
#include "Meta/TypeTraits.hpp"
#include "Meta/Concepts.hpp"
#include "Util/Bits.hpp"



template<CC::Integer I, SizeT N>
	requires (N < Bits::bitCount<I>() && N != 0)
class FixedPoint {

public:

	using IntegerT = I;

	struct SplitResult {
		I integer;
		I fractional;
	};

	constexpr static SizeT Shift = N;
	constexpr static SizeT FractionBits = N;


	constexpr FixedPoint() noexcept : i(0) {}

	template<CC::Integral J>
	constexpr FixedPoint(J j) noexcept : i(j << Shift) {}

	template<CC::Float F>
	constexpr FixedPoint(F f) noexcept(CC::IEEEMaskableFloat<F>) : i(0) { set(f); }

	template<CC::Integer J0, CC::Integer J1>
	constexpr FixedPoint(J0 integer, J1 fraction) noexcept : i((integer << Shift) | asReciprocal(fraction).i) {}

	template<CC::Integer J, SizeT F>
	constexpr explicit FixedPoint(FixedPoint<J, F> x) noexcept : i(0) { set(x); }


	template<CC::Integer J>
	constexpr FixedPoint& add(J x) noexcept {
		i += x << Shift;
		return *this;
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& add(FixedPoint<J, F> x) noexcept {
		i += FixedPoint<I, Shift>(x).i;
		return *this;
	}

	template<CC::Integer J>
	constexpr FixedPoint& subtract(J x) noexcept {
		i -= x << Shift;
		return *this;
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& subtract(FixedPoint<J, F> x) noexcept {
		i -= FixedPoint<I, Shift>(x).i;
		return *this;
	}

	template<CC::Integer J>
	constexpr FixedPoint& multiply(J x) noexcept {
		i *= x;
		return *this;
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& multiply(FixedPoint<J, F> x) noexcept {

		using X = TT::MaxType<I, J>;

		if constexpr (TT::HasBiggerType<X>) {

			using K = TT::BiggerType<X>;

			K k = K(x.i) * i;
			k += K(1) << (Shift + F - 1);
			k >>= F;

			i = k;

		} else {

			constexpr SizeT XBits = Bits::bitCount<X>();

			OverflowMultiplyResult<X> result = CC::SignedType<I> ? Math::overflowSignedMultiply<X>(i, x.i) : Math::overflowUnsignedMultiply<X>(i, x.i);

			constexpr SizeT BiasShift = Shift + F - 1;

			if constexpr (BiasShift >= XBits) {
				result.top += X(1) << (BiasShift - XBits);
			} else {
				result.bottom += X(1) << BiasShift;
			}

			if constexpr (F >= XBits) {
				i = result.top >> (F - XBits);
			} else {
				i = (result.top << (XBits - F)) | (result.bottom >> F);
			}

		}

		return *this;

	}

	template<CC::Integer J>
	constexpr FixedPoint& divide(J x) noexcept {
		i /= x;
		return *this;
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& divide(FixedPoint<J, F> x) noexcept(TT::HasBiggerType<TT::MaxType<I, J>>) {

		using X = TT::MaxType<I, J>;

		if constexpr (TT::HasBiggerType<X>) {

			using K = TT::BiggerType<X>;

			K k = K(i) << F;
			i = k / x.i;

		} else {

			BigInt b = i;
			b <<= F;
			i = (b / x.i).template toInteger<I>();

		}

		return *this;

	}

	template<CC::Integer J>
	constexpr FixedPoint& mod(J x) noexcept {
		i %= x;
		return *this;
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& mod(FixedPoint<J, F> x) noexcept(TT::HasBiggerType<TT::MaxType<I, J>>) {

		using X = TT::MaxType<I, J>;

		if constexpr (TT::HasBiggerType<X>) {

			using K = TT::BiggerType<X>;

			K k = K(i) << F;
			i = k % x.i;

		} else {

			BigInt b = i;
			b <<= F;
			i = (b % x.i).template toInteger<I>();

		}

		return *this;

	}

	constexpr FixedPoint& shiftLeft(SizeT n) noexcept {
		i <<= n;
		return *this;
	}

	constexpr FixedPoint& shiftRight(SizeT n) noexcept {
		i >>= n;
		return *this;
	}


	template<CC::Integer J>
	constexpr FixedPoint& bitAnd(J x) noexcept {
		i &= x;
		return *this;
	}

	template<CC::Integer J>
	constexpr FixedPoint& bitOr(J x) noexcept {
		i |= x;
		return *this;
	}

	template<CC::Integer J>
	constexpr FixedPoint& bitXor(J x) noexcept {
		i ^= x;
		return *this;
	}


	constexpr FixedPoint& bitAnd(FixedPoint x) noexcept {
		return bitAnd(x.i);
	}

	constexpr FixedPoint& bitOr(FixedPoint x) noexcept {
		return bitOr(x.i);
	}

	constexpr FixedPoint& bitXor(FixedPoint x) noexcept {
		return bitXor(x.i);
	}


	constexpr FixedPoint complement() const noexcept {
		return ~i;
	}

	constexpr FixedPoint abs() const noexcept {
		return Math::abs(i);
	}

	constexpr FixedPoint negate() const noexcept {
		return -i;
	}


	template<CC::Arithmetic A>
	constexpr FixedPoint& operator=(A x) noexcept {
		set(x);
		return *this;
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& operator=(FixedPoint<J, F> x) noexcept {
		set(x);
		return *this;
	}

	template<CC::Integer J>
	constexpr FixedPoint& operator+=(J x) noexcept {
		return add(x);
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& operator+=(FixedPoint<J, F> x) noexcept {
		return add(x);
	}

	template<CC::Integer J>
	constexpr FixedPoint& operator-=(J x) noexcept {
		return subtract(x);
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& operator-=(FixedPoint<J, F> x) noexcept {
		return subtract(x);
	}

	template<CC::Integer J>
	constexpr FixedPoint& operator*=(J x) noexcept {
		return multiply(x);
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& operator*=(FixedPoint<J, F> x) noexcept {
		return multiply(x);
	}

	template<CC::Integer J>
	constexpr FixedPoint& operator/=(J x) noexcept {
		return divide(x);
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& operator/=(FixedPoint<J, F> x) noexcept(noexcept(divide(x))) {
		return divide(x);
	}

	template<CC::Integer J>
	constexpr FixedPoint& operator%=(J x) noexcept {
		return mod(x);
	}

	template<CC::Integer J, SizeT F>
	constexpr FixedPoint& operator%=(FixedPoint<J, F> x) noexcept(noexcept(mod(x))) {
		return mod(x);
	}

	constexpr FixedPoint operator+() const noexcept {
		return *this;
	}

	constexpr FixedPoint operator-() const noexcept {
		return negate();
	}

	constexpr FixedPoint operator<<(SizeT n) const noexcept {
		FixedPoint x = *this;
		return x.shiftLeft(n);
	}

	constexpr FixedPoint operator>>(SizeT n) const noexcept {
		FixedPoint x = *this;
		return x.shiftRight(n);
	}

	constexpr FixedPoint& operator<<=(SizeT n) noexcept {
		return shiftLeft(n);
	}

	constexpr FixedPoint& operator>>=(SizeT n) noexcept {
		return shiftRight(n);
	}

	template<CC::Integer A>
	constexpr FixedPoint& operator&=(A x) noexcept {
		return bitAnd(x);
	}

	template<CC::Integer A>
	constexpr FixedPoint& operator|=(A x) noexcept {
		return bitOr(x);

	}

	template<CC::Integer A>
	constexpr FixedPoint& operator^=(A x) noexcept {
		return bitXor(x);

	}

	constexpr FixedPoint operator~() const noexcept {
		return ~i;
	}


	template<CC::Integral J>
	constexpr void set(J x) noexcept {
		i = x << Shift;
	}

	template<CC::Float F>
	constexpr void set(F x) noexcept(CC::IEEEMaskableFloat<F>) {

		auto [integer, fract] = Math::split(x);
		i = (static_cast<I>(integer) << Shift) | static_cast<I>(fract * (1 << Shift));

	}

	template<CC::Integer J, SizeT F>
	constexpr void set(FixedPoint<J, F> f) noexcept {

		if constexpr (Shift == F) {
			i = f.i;
		} else if constexpr (Shift > F) {
			i = I(f.i) << (Shift - F);
		} else {
			f += J(1) << (F - Shift - 1);
			i = I(f.i) >> (F - Shift);
		}

	}


	constexpr I raw() const noexcept {
		return i;
	}

	constexpr I trunc() const noexcept {
		return i >> Shift;
	}

	constexpr I fract() const noexcept {
		return Bits::mask(i, 0, Shift);
	}

	constexpr SplitResult split() const noexcept {
		return { trunc(), fract() };
	}

	constexpr bool isNegative() const noexcept {
		return i >> (Bits::bitCount<I>() - 1);
	}


	template<CC::Integer J>
	constexpr J toInteger() const noexcept {
		return trunc();
	}

	template<CC::Float F>
	constexpr F toFloat() const noexcept {

		auto [a, b] = split();
		return F(a) + F(b) / (I(1) << Shift);

	}

	constexpr I toRaw() const noexcept {
		return i;
	}


	template<CC::Arithmetic A>
	constexpr explicit operator A() const noexcept {

		if constexpr (CC::Integral<A>) {
			return toInteger<A>();
		} else {
			return toFloat<A>();
		}

	}


	template<CC::Arithmetic A>
	constexpr auto operator<=>(A x) const noexcept {
		return *this <=> FixedPoint(x);
	}

	template<CC::Integer J, SizeT F>
	constexpr auto operator<=>(FixedPoint<J, F> x) const noexcept {

		using X = TT::MaxType<I, J>;

		if constexpr (Shift == F) {
			return i <=> x.i;
		} else if constexpr (Shift > F && TT::HasBiggerType<J>) {
			return i <=> TT::BiggerType<J>(x.i) << (Shift - F);
		} else if constexpr (Shift < F && TT::HasBiggerType<I>) {
			return TT::BiggerType<I>(i) << (F - Shift) <=> x.i;
		}

		if (trunc() != x.trunc()) {
			return trunc() <=> x.trunc();
		}

		I a = fract();
		J b = x.fract();

		if constexpr (Shift == F) {
			return a <=> b;
		} else if constexpr (Shift > F) {
			return a <=> I(b) << (Shift - F);
		} else {
			return J(a) << (F - Shift) <=> b;
		}

	}


	constexpr std::string toString() const noexcept {

		auto [integer, fract] = split();

		std::string str = std::to_string(integer);

		if (fract == 0) {
			return str;
		}

		str += '.';

		while (fract != 0) {

			I n = fract * 10;
			fract = Bits::mask(n, 0, Shift);

			str += '0' + (n >> Shift);

		}

		return str;

	}


	constexpr static FixedPoint fromRaw(I i) noexcept {

		FixedPoint f;
		f.i = i;

		return f;

	}

	constexpr static FixedPoint min() noexcept {

		if constexpr (CC::SignedType<I>) {
			return fromRaw(-1);
		} else {
			return fromRaw(0);
		}

	}

	constexpr static FixedPoint max() noexcept {

		if constexpr (CC::SignedType<I>) {
			return fromRaw(Bits::ones<I>(Bits::bitCount<I>() - 1));
		} else {
			return fromRaw(Bits::allOnes<I>());
		}

	}


	template<CC::Integer J>
	constexpr static FixedPoint asReciprocal(J denominator) {
		return FixedPoint(1) / FixedPoint(denominator);
	}

private:

	template<CC::Integer J, SizeT F>
		requires (F < Bits::bitCount<J>() && F != 0)
	friend class FixedPoint;


	I i;

};


template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator+(FixedPoint<I, F> a, J b) noexcept {
	return a.add(b);
}

template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator+(J a, FixedPoint<I, F> b) noexcept {
	return b.add(a);
}

template<CC::Integer I, CC::Integer J, SizeT F, class X = FixedPoint<TT::MaxType<I, J>, F>>
constexpr X operator+(FixedPoint<I, F> a, FixedPoint<J, F> b) noexcept {
	return X(a).add(X(b));
}


template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator-(FixedPoint<I, F> a, J b) noexcept {
	return a.subtract(b);
}

template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator-(J a, FixedPoint<I, F> b) noexcept {
	return b.negate().add(a);
}

template<CC::Integer I, CC::Integer J, SizeT F, class X = FixedPoint<TT::MaxType<I, J>, F>>
constexpr X operator-(FixedPoint<I, F> a, FixedPoint<J, F> b) noexcept {
	return X(a).subtract(X(b));
}


template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator*(FixedPoint<I, F> a, J b) noexcept {
	return a.multiply(b);
}

template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator*(J a, FixedPoint<I, F> b) noexcept {
	return b.multiply(a);
}

template<CC::Integer I, CC::Integer J, SizeT F, class X = FixedPoint<TT::MaxType<I, J>, F>>
constexpr X operator*(FixedPoint<I, F> a, FixedPoint<J, F> b) noexcept {
	return X(a).multiply(X(b));
}


template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator/(FixedPoint<I, F> a, J b) noexcept {
	return a.divide(b);
}

template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator/(J a, FixedPoint<I, F> b) {
	return FixedPoint<I, F>(a).divide(b);
}

template<CC::Integer I, CC::Integer J, SizeT F, class X = FixedPoint<TT::MaxType<I, J>, F>>
constexpr X operator/(FixedPoint<I, F> a, FixedPoint<J, F> b) {
	return X(a).divide(X(b));
}


template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator%(FixedPoint<I, F> a, J b) noexcept {
	return a.mod(b);
}

template<CC::Integer I, SizeT F, CC::Integer J>
constexpr FixedPoint<I, F> operator%(J a, FixedPoint<I, F> b) {
	return FixedPoint<I, F>(a).mod(b);
}

template<CC::Integer I, CC::Integer J, SizeT F, class X = FixedPoint<TT::MaxType<I, J>, F>>
constexpr X operator%(FixedPoint<I, F> a, FixedPoint<J, F> b) {
	return X(a).mod(X(b));
}


template<CC::Integer I, SizeT F, class U> requires (CC::Integer<U> || CC::Equal<U, FixedPoint<I, F>>)
constexpr FixedPoint<I, F> operator&(FixedPoint<I, F> a, U b) noexcept {
	return a.bitAnd(b);
}

template<CC::Integer I, SizeT F, CC::Integer T>
constexpr FixedPoint<I, F> operator&(T a, FixedPoint<I, F> b) noexcept {
	return b.bitAnd(a);
}


template<CC::Integer I, SizeT F, class U> requires (CC::Integer<U> || CC::Equal<U, FixedPoint<I, F>>)
constexpr FixedPoint<I, F> operator|(FixedPoint<I, F> a, U b) noexcept {
	return a.bitOr(b);
}

template<CC::Integer I, SizeT F, CC::Integer T>
constexpr FixedPoint<I, F> operator|(T a, FixedPoint<I, F> b) noexcept {
	return b.bitOr(a);
}


template<CC::Integer I, SizeT F, class U> requires (CC::Integer<U> || CC::Equal<U, FixedPoint<I, F>>)
constexpr FixedPoint<I, F> operator^(FixedPoint<I, F> a, U b) noexcept {
	return a.bitXor(b);
}

template<CC::Integer I, SizeT F, CC::Integer T>
constexpr FixedPoint<I, F> operator^(T a, FixedPoint<I, F> b) noexcept {
	return b.bitXor(a);
}


template<CC::Integer I, SizeT F>
inline RawLog& operator<<(RawLog& log, const FixedPoint<I, F>& fix) {
	return log << fix.toString();
}
