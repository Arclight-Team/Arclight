/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 fixedpoint.hpp
 */

#pragma once

#include "math.hpp"
#include "bigint.hpp"
#include "overflow.hpp"
#include "common/typetraits.hpp"
#include "common/concepts.hpp"
#include "util/bits.hpp"



template<CC::Integral I, SizeT Fract>
	requires (Fract < Bits::bitCount<I>() && Fract != 0)
class FixedPoint {

public:

	struct SplitResult {
		I integer;
		I fractional;
	};

	constexpr static SizeT Shift = Fract;

	constexpr FixedPoint() noexcept : i(0) {}

	template<CC::Integral J>
	constexpr FixedPoint(J j) noexcept : i(j << Shift) {}

	template<CC::Float F>
	constexpr FixedPoint(F f) noexcept : i(0) { set(f); }

	template<CC::Integral J0, CC::Integral J1>
	constexpr FixedPoint(J0 integer, J1 fraction) noexcept : i((integer << Shift) | fraction) {}

	template<CC::Integral J, SizeT F>
	constexpr explicit FixedPoint(FixedPoint<J, F> x) : i(0) { set(x); }


	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& add(FixedPoint<J, F> x) noexcept {
		i += FixedPoint<I, Fract>(x).i;
		return *this;
	}

	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& subtract(FixedPoint<J, F> x) noexcept {
		i -= FixedPoint<I, Fract>(x).i;
		return *this;
	}

	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& multiply(FixedPoint<J, F> x) noexcept {

		using X = TT::MaxType<I, J>;

		if constexpr (TT::HasBiggerType<X>) {

			using K = TT::BiggerType<X>;

			K k = x.i * i;
			k += K(1) << (Fract + F - 1);
			k >>= F;

			i = k;

		} else {

			constexpr bool Signed = CC::SignedType<I> || CC::SignedType<J>;
			constexpr SizeT XBits = Bits::bitCount<X>();

			OverflowMultiplyResult<X> result = CC::SignedType<I> ? Math::overflowSignedMultiply<X>(i, x.i) : Math::overflowUnsignedMultiply<X>(i, x.i);

			constexpr SizeT biasShift = Fract + F - 1;

			if constexpr (biasShift >= XBits) {
				result.top += X(1) << (biasShift - XBits);
			} else {
				result.bottom += X(1) << biasShift;
			}

			if constexpr (F >= XBits) {
				i = result.top >> (F - XBits);
			} else {
				i = (result.top << (XBits - F)) | (result.bottom >> F);
			}

		}

		return *this;

	}

	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& divide(FixedPoint<J, F> x) noexcept(TT::HasBiggerType<I> && TT::HasBiggerType<J>){

		using X = TT::Conditional<sizeof(I) >= sizeof(J), I, J>;

		if constexpr (TT::HasBiggerType<X>) {

			using K = TT::BiggerType<X>;

			K k = i << F;
			i = k / x.i;

		} else {

			BigInt b = i;
			b <<= F;
			i = (b / x.i).toInteger<I>();

		}

		return *this;

	}

	constexpr FixedPoint& shiftLeft(SizeT n) noexcept {
		i <<= n;
		return *this;
	}

	constexpr FixedPoint& shiftRight(SizeT n) noexcept {
		i <<= n;
		return *this;
	}


	template<CC::Integral J>
	constexpr FixedPoint& bitAnd(J x) noexcept {
		i &= x;
		return *this;
	}

	template<CC::Integral J>
	constexpr FixedPoint& bitOr(J x) noexcept {
		i |= x;
		return *this;
	}

	template<CC::Integral J>
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

	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& operator=(FixedPoint<J, F> x) noexcept {
		set(x);
		return *this;
	}

	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& operator+=(FixedPoint<J, F> x) noexcept {
		return add(x);
	}

	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& operator-=(FixedPoint<J, F> x) noexcept {
		return subtract(x);
	}

	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& operator*=(FixedPoint<J, F> x) noexcept {
		return multiply(x);
	}

	template<CC::Integral J, SizeT F>
	constexpr FixedPoint& operator/=(FixedPoint<J, F> x) noexcept(noexcept(divide(x))) {
		return divide(x);
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

	template<CC::Integral A>
	constexpr FixedPoint& operator&=(A x) noexcept {
		return bitAnd(x);
	}

	template<CC::Integral A>
	constexpr FixedPoint& operator|=(A x) noexcept {
		return bitOr(x);

	}

	template<CC::Integral A>
	constexpr FixedPoint& operator^=(A x) noexcept {
		return bitXor(x);

	}

	constexpr FixedPoint operator~() const noexcept {
		return ~i;
	}

	template<CC::Arithmetic A>
	constexpr void set(A x) noexcept {

		if constexpr (CC::Integral<A>) {

			i = x << Shift;

		} else {

			auto [integer, fract] = Math::split(x);
			i = (static_cast<I>(integer) << Shift) | static_cast<I>(fract * (1 << Shift));

		}

	}

	template<CC::Integral J, SizeT F>
	constexpr void set(FixedPoint<J, F> f) noexcept {

		if constexpr (Fract == F) {
			i = f.i;
		} else if constexpr (Fract > F) {
			i = I(f.i) << (Fract - F);
		} else {
			f += J(1) << (F - Fract - 1);
			i = I(f.i) >> (F - Fract);
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

	template<CC::Integral J>
	constexpr J toInteger() const noexcept {
		return trunc();
	}

	template<CC::Float F>
	constexpr F toFloat() const noexcept {

		auto [a, b] = split();
		return F(a) + F(b) / (I(1) << Shift);

	}


	template<CC::Arithmetic A>
	constexpr explicit operator A() const noexcept {

		if constexpr (CC::Integral<A>) {
			return toInteger<A>();
		} else {
			return toFloat<A>();
		}

	}


	template<CC::Integral J, SizeT F>
	constexpr auto operator<=>(FixedPoint<J, F> x) const noexcept {

		using X = TT::MaxType<I, J>;

		if constexpr (Fract == F) {
			return i <=> x.i;
		} else if (Fract > F && TT::HasBiggerType<J>) {
			return i <=> TT::BiggerType<J>(x.i) << (Fract - F);
		} else if (Fract < F && TT::HasBiggerType<I>) {
			return TT::BiggerType<I>(i) << (F - Fract) <=> x.i;
		} else {

			if (trunc<I>() != x.trunc<J>()) {
				return trunc<I>() <=> x.trunc<J>();
			}

			I a = fract();
			J b = x.fract();

			if constexpr (Fract == F) {
				return a <=> b;
			} else if constexpr (Fract > F) {
				return a <=> I(b) << (Fract - F);
			} else {
				return J(a) << (F - Fract) <=> b;
			}

		}

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

private:

	I i;

};



template<CC::Integral I, SizeT F, CC::Integral J, SizeT G, class X = FixedPoint<TT::MaxType<I, J>, Math::max(F, G)>>
constexpr X operator+(FixedPoint<I, F> a, FixedPoint<J, G> b) noexcept {

	X x = a;
	x += b;

	return x;

}

template<CC::Integral I, SizeT F, CC::Integral J, SizeT G, class X = FixedPoint<TT::MaxType<I, J>, Math::max(F, G)>>
constexpr X operator-(FixedPoint<I, F> a, FixedPoint<J, G> b) noexcept {

	X x = a;
	x -= b;

	return x;

}

template<CC::Integral I, SizeT F, CC::Integral J, SizeT G, class X = FixedPoint<TT::MaxType<I, J>, Math::max(F, G)>>
constexpr X operator*(FixedPoint<I, F> a, FixedPoint<J, G> b) noexcept {

	X x = a;
	x *= b;

	return x;

}

template<CC::Integral I, SizeT F, CC::Integral J, SizeT G, class X = FixedPoint<TT::MaxType<I, J>, Math::max(F, G)>>
constexpr X operator/(FixedPoint<I, F> a, FixedPoint<J, G> b) noexcept(noexcept(a /= b)) {

	X x = a;
	x /= b;

	return x;

}

template<CC::Integral I, SizeT F, class U> requires (CC::Integral<U> || CC::Equal<U, FixedPoint<I, F>>)
constexpr FixedPoint<I, F> operator&(FixedPoint<I, F> a, U b) noexcept {
	return a.bitAnd(b);
}

template<CC::Integral I, SizeT F, CC::Integral T>
constexpr FixedPoint<I, F> operator&(T a, FixedPoint<I, F> b) noexcept {
	return b.bitAnd(a);
}

template<CC::Integral I, SizeT F, class U> requires (CC::Integral<U> || CC::Equal<U, FixedPoint<I, F>>)
constexpr FixedPoint<I, F> operator|(FixedPoint<I, F> a, U b) noexcept {
	return a.bitOr(b);
}

template<CC::Integral I, SizeT F, CC::Integral T>
constexpr FixedPoint<I, F> operator|(T a, FixedPoint<I, F> b) noexcept {
	return b.bitOr(a);
}

template<CC::Integral I, SizeT F, class U> requires (CC::Integral<U> || CC::Equal<U, FixedPoint<I, F>>)
constexpr FixedPoint<I, F> operator^(FixedPoint<I, F> a, U b) noexcept {
	return a.bitXor(b);
}

template<CC::Integral I, SizeT F, CC::Integral T>
constexpr FixedPoint<I, F> operator^(T a, FixedPoint<I, F> b) noexcept {
	return b.bitXor(a);
}