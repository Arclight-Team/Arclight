/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 overflow.hpp
 */

#pragma once

#include "util/bits.hpp"



template<CC::Arithmetic A>
struct OverflowAddResult {

	constexpr OverflowAddResult() noexcept : value(0), overflown(false) {}

	A value;
	bool overflown;

};

template<CC::Arithmetic A>
struct OverflowMultiplyResult {

	constexpr OverflowMultiplyResult() noexcept : bottom(0), top(0), overflown(false) {}

	A bottom;
	A top;
	bool overflown;

};


namespace Math {

	template<CC::Arithmetic A>
	constexpr OverflowAddResult<A> overflowSignedAdd(A a, A b) noexcept {

		using UnsignedT = TT::MakeUnsigned<A>;

		OverflowAddResult<A> result;

		UnsignedT c = static_cast<UnsignedT>(a) + static_cast<UnsignedT>(b);
		result.value = c;
		result.overflown = ~(a ^ b) & (a ^ c) & static_cast<A>(UnsignedT(1) << (Bits::bitCount<A>() - 1));

		return result;

	}

	template<CC::Arithmetic A>
	constexpr OverflowAddResult<A> overflowUnsignedAdd(A a, A b) noexcept {

		using UnsignedT = TT::MakeUnsigned<A>;

		UnsignedT ua = static_cast<UnsignedT>(a);
		UnsignedT c = ua + static_cast<UnsignedT>(b);

		OverflowAddResult<A> result;
		result.value = c;
		result.overflown = c < ua;

		return result;

	}

	template<CC::Arithmetic A>
	constexpr OverflowAddResult<A> overflowAdd(A a, A b) noexcept {

		if constexpr (CC::UnsignedType<A>) {
			return overflowUnsignedAdd(a, b);
		} else {
			return overflowSignedAdd(a, b);
		}

	}

	template<CC::Arithmetic A>
	constexpr OverflowAddResult<A> overflowSignedSubtract(A a, A b) noexcept {

		using UnsignedT = TT::MakeUnsigned<A>;

		OverflowAddResult<A> result;

		UnsignedT c = static_cast<UnsignedT>(a) + static_cast<UnsignedT>(b);
		result.value = c;
		result.overflown = (a ^ b) & (a ^ c) & static_cast<A>(UnsignedT(1) << (Bits::bitCount<A>() - 1));

		return result;

	}

	template<CC::Arithmetic A>
	constexpr OverflowAddResult<A> overflowUnsignedSubtract(A a, A b) noexcept {

		using UnsignedT = TT::MakeUnsigned<A>;

		UnsignedT ua = static_cast<UnsignedT>(a);
		UnsignedT ub = static_cast<UnsignedT>(b);

		OverflowAddResult<A> result;
		result.value = ua - ub;
		result.overflown = ua < ub;

		return result;

	}

	template<CC::Arithmetic A>
	constexpr OverflowAddResult<A> overflowSubtract(A a, A b) noexcept {

		if constexpr (CC::UnsignedType<A>) {
			return overflowUnsignedSubtract(a, b);
		} else {
			return overflowSignedSubtract(a, b);
		}

	}

	template<CC::Arithmetic A>
	constexpr OverflowMultiplyResult<A> overflowSignedMultiply(A a, A b) noexcept {

		using SignedT = TT::MakeSigned<A>;
		using UnsignedT = TT::MakeUnsigned<A>;

		SignedT sa = static_cast<SignedT>(a);
		SignedT sb = static_cast<SignedT>(b);

		OverflowMultiplyResult<A> result;

		if constexpr (TT::HasBiggerType<SignedT>) {

			using BiggerT = TT::BiggerType<SignedT>;
			BiggerT c = static_cast<BiggerT>(sa) * sb;

			result.bottom = c & Bits::allOnes<SignedT>();
			result.top = c >> Bits::bitCount<SignedT>();

		} else {

			constexpr SizeT halfBits = Bits::bitCount<UnsignedT>() / 2;

			// Dissect via arithmetic shifts, cast to unsigned
			UnsignedT al = sa & Bits::mask(sa, 0, halfBits);
			UnsignedT ah = sa >> halfBits;
			UnsignedT bl = sb & Bits::mask(sb, 0, halfBits);
			UnsignedT bh = sb >> halfBits;

			// Unsigned partial products
			UnsignedT x0 = al * bl;
			UnsignedT x1 = ah * bl;
			UnsignedT x2 = al * bh;
			UnsignedT x3 = ah * bh;

			// Identical to unsigned version
			UnsignedT totalCarry = 0;

			OverflowAddResult carryAdd = overflowAdd(x0, x1 << halfBits);
			totalCarry += carryAdd.overflown;

			carryAdd = overflowAdd(carryAdd.value, x2 << halfBits);
			totalCarry += carryAdd.overflown;

			result.bottom = carryAdd.value;
			result.top = (x1 >> halfBits) + (x2 >> halfBits) + x3;

		}

		result.overflown = result.top != (static_cast<SignedT>(result.bottom) >> (Bits::bitCount<SignedT>() - 1));

		return result;

	}

	template<CC::Arithmetic A>
	constexpr OverflowMultiplyResult<A> overflowUnsignedMultiply(A a, A b) noexcept {

		using UnsignedT = TT::MakeUnsigned<A>;

		UnsignedT ua = static_cast<UnsignedT>(a);
		UnsignedT ub = static_cast<UnsignedT>(b);

		OverflowMultiplyResult<A> result;

		if constexpr (TT::HasBiggerType<UnsignedT>) {

			using BiggerT = TT::BiggerType<UnsignedT>;
			BiggerT c = static_cast<BiggerT>(ua) * ub;

			result.bottom = c & Bits::allOnes<UnsignedT>();
			result.top = c >> Bits::bitCount<UnsignedT>();

		} else {

			constexpr SizeT halfBits = Bits::bitCount<UnsignedT>() / 2;

			// Dissect to half-sized integers
			UnsignedT al = ua & Bits::mask(ua, 0, halfBits);
			UnsignedT ah = ua >> halfBits;
			UnsignedT bl = ub & Bits::mask(ub, 0, halfBits);
			UnsignedT bh = ub >> halfBits;

			// Calculate half products
			UnsignedT x0 = al * bl;
			UnsignedT x1 = ah * bl;
			UnsignedT x2 = al * bh;
			UnsignedT x3 = ah * bh;

			UnsignedT totalCarry = 0;

			// Add lower product + first mid partial product
			OverflowAddResult carryAdd = overflowAdd(x0, x1 << halfBits);
			totalCarry += carryAdd.overflown;

			// Add the result to the second mid partial product
			carryAdd = overflowAdd(carryAdd.value, x2 << halfBits);
			totalCarry += carryAdd.overflown;

			// Bottom part is our previous result, top part is the sum of the two mid partial products, the upper product and the carry from bottom
			result.bottom = carryAdd.value;
			result.top = (x1 >> halfBits) + (x2 >> halfBits) + x3 + totalCarry;

		}

		// If top is non-zero, an overflow occured
		result.overflown = result.top;

		return result;

	}
	
	template<CC::Arithmetic A>
	constexpr OverflowMultiplyResult<A> overflowMultiply(A a, A b) noexcept {

		if constexpr (CC::UnsignedType<A>) {
			return overflowUnsignedMultiply(a, b);
		} else {
			return overflowSignedMultiply(a, b);
		}

	}

}
