/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bigint.hpp
 */

#pragma once

#include "math/overflow.hpp"
#include "util/assert.hpp"
#include "util/bits.hpp"
#include "types.hpp"

#include <vector>
#include <span>



class BigInt {

	using ValueT = u32;
	constexpr static u32 ValueBits = Bits::bitCount<ValueT>();

	template<class T>
	struct _DivResult {
		T quotient;
		T remainder;
	};

public:

	using DivResult = _DivResult<BigInt>;


	constexpr BigInt() : signum(0), magnitude(1) {}

	constexpr BigInt(i32 sign, std::span<const ValueT> magnitude) : signum(sign), magnitude(magnitude.size()) {

		std::copy(magnitude.begin(), magnitude.end(), this->magnitude.begin());

		bool qualifiesZero = magnitude.size() == 1 && magnitude.front() == 0;

		if (qualifiesZero) {
			signum = 0;		//Actually sign of 0
		} else if (signum == 0) {
			signum = 1;		//Someone messed with us
		}

	}

	template<Integer I>
	constexpr BigInt(I i) {
		set(i);
	}

	explicit BigInt(std::string_view view) {
		*this = parseString(view);
	}

	template<Integer I>
	constexpr void set(I i) {

		if (i == I(0)) {

			setZero();

		} else {

			SizeT bits = Bits::bitCount<I>() - Bits::clz(i);

			signum = i >= I(0) ? 1 : -1;
			magnitude.resize((bits + ValueBits - 1) / ValueBits);
			Bits::disassemble(Math::abs(i), magnitude.data(), magnitude.size());

		}

	}

	constexpr BigInt& add(const BigInt& b) {

		*this = addCore(*this, b);
		return *this;

	}

	constexpr BigInt& subtract(const BigInt& b) {

		*this = subtractCore(*this, b);
		return *this;

	}

	constexpr BigInt& multiply(const BigInt& b) {

		*this = multiplyCore(*this, b);
		return *this;

	}

	constexpr BigInt& divide(const BigInt& b) {

		*this = divideCore(*this, b).quotient;
		return *this;

	}

	constexpr BigInt& mod(const BigInt& b) {

		*this = divideCore(*this, b).remainder;
		return *this;

	}

	constexpr DivResult divmod(const BigInt& b) {
		return divideCore(*this, b);
	}


	constexpr BigInt& increment() {

		switch (signum) {

			case -1:
				decrementMagnitude();
				checkZero();
				break;

			case 0:
				incrementMagnitude();
				signum = 1;
				break;

			case 1:
				incrementMagnitude();
				break;

		}

		return *this;

	}

	constexpr BigInt& decrement() {

		switch (signum) {

			case -1:
				incrementMagnitude();
				break;

			case 0:
				incrementMagnitude();
				signum = -1;
				break;

			case 1:
				decrementMagnitude();
				checkZero();
				break;

		}

		return *this;

	}


	constexpr BigInt& shiftLeft(SizeT count) {

		SizeT shiftCount = count % ValueBits;
		SizeT insertCount = count / ValueBits;

		if (isZero()) {
			return *this;
		}

		magnitude.insert(magnitude.begin(), insertCount, 0);

		if (shiftCount) {

			ValueT shiftOut = 0;

			for (SizeT i = insertCount; i < magnitude.size(); i++) {

				ValueT& v = magnitude[i];
				ValueT nextShiftOut = v >> (ValueBits - shiftCount);

				v = shiftOut | (v << shiftCount);
				shiftOut = nextShiftOut;

			}

			if (shiftOut) {
				magnitude.emplace_back(shiftOut);
			}

		}

		return *this;

	}

	constexpr BigInt& shiftRight(SizeT count) {

		if (isZero()) {
			return *this;
		}

		if (count >= magnitudeBitSize()) {

			setZero();

		} else {

			SizeT shiftCount = count % ValueBits;
			SizeT eraseCount = count / ValueBits;

			if (eraseCount) {

				std::move(magnitude.begin() + eraseCount, magnitude.end(), magnitude.begin());
				magnitude.resize(magnitude.size() - eraseCount);

			}

			if (shiftCount) {

				for (SizeT i = 0; i < magnitude.size() - 1; i++) {

					ValueT& v = magnitude[i];
					v = magnitude[i + 1] << (ValueBits - shiftCount) | v >> shiftCount;

				}

				magnitude.back() >>= shiftCount;

			}

			compress();

		}

		return *this;

	}


	constexpr BigInt pow(const BigInt& exp) {

		*this = powCore(*this, exp);
		return *this;

	}

	constexpr BigInt abs() const noexcept {
		return BigInt(Math::abs(signum), magnitude);
	}

	constexpr BigInt negate() const noexcept {
		return BigInt(-signum, magnitude);
	}

	constexpr bool lowestBitSet() const {
		return magnitude.front() & 1;
	}


	constexpr bool isZero() const noexcept {
		return signum == 0;
	}

	constexpr bool isPositive() const noexcept {
		return signum >= 0;
	}

	constexpr bool isNegative() const noexcept {
		return signum == -1;
	}

	constexpr i32 sign() const noexcept {
		return signum;
	}


	constexpr BigInt& operator+=(const BigInt& b) {
		return add(b);
	}

	constexpr BigInt& operator-=(const BigInt& b) {
		return subtract(b);
	}

	constexpr BigInt& operator*=(const BigInt& b) {
		return multiply(b);
	}

	constexpr BigInt& operator/=(const BigInt& b) {
		return divide(b);
	}

	constexpr BigInt& operator%=(const BigInt& b) {
		return mod(b);
	}

	constexpr BigInt& operator++() {
		return increment();
	}

	constexpr BigInt& operator--() {
		return decrement();
	}

	constexpr BigInt operator++(i32) {

		BigInt b = *this;
		++*this;

		return b;

	}

	constexpr BigInt operator--(i32) {

		BigInt b = *this;
		--*this;

		return b;

	}

	constexpr BigInt operator-() const {
		return negate();
	}

	constexpr explicit operator bool() const noexcept {
		return !isZero();
	}

	constexpr auto operator==(const BigInt& b) const {

		if (sign() != b.sign() || magnitude.size() != b.magnitude.size()) {
			return false;
		}

		for (SizeT i = 0; i < magnitude.size(); i++) {

			if (magnitude[i] != b.magnitude[i]) {
				return false;
			}

		}

		return true;

	}

	constexpr auto operator<=>(const BigInt& b) const {

		if (sign() != b.sign()) {
			return sign() <=> b.sign();
		}

		//Signs are equal
		if (magnitude.size() != b.magnitude.size()) {
			return isPositive() ? magnitude.size() <=> b.magnitude.size() : b.magnitude.size() <=> magnitude.size();
		}

		//Magnitude range is equal
		for (SizeT i = 0; i < magnitude.size(); i++) {

			SizeT j = magnitude.size() - i - 1;

			ValueT va = magnitude[j];
			ValueT vb = b.magnitude[j];

			if (va != vb) {
				return isPositive() ? va <=> vb : vb <=> va;
			}

		}

		return std::strong_ordering::equal;

	}

	template<Integer I>
	constexpr I toInteger() const {

		if constexpr (sizeof(I) <= sizeof(ValueT)) {

			return isNegative() ? -magnitude.front() : magnitude.front();

		} else {

			I i = Bits::assemble<I>(magnitude.data(), magnitude.size());

			if (isNegative()) {
				i = -i;
			}

			return i;

		}

	}

	template<Integer I>
	constexpr bool hasIntegerRepresentation() const {

		if constexpr (UnsignedType<I>) {

			return isPositive() && magnitudeBitSize() <= Bits::bitCount<I>();

		} else {

			if (magnitudeBitSize() + 1 <= Bits::bitCount<I>()) {
				return true;
			}

			//Cover minimum value
			if (isNegative()) {

				SizeT bits = magnitudeBitSize();
				ValueT upper = ValueT(1) << ((bits % ValueBits) - 1);

				for (SizeT i = 0; i < bits / ValueBits; i++) {

					if (magnitude[i]) {
						return false;
					}

				}

				return magnitude.back() == upper;

			}

			return false;

		}

	}

	constexpr SizeT magnitudeBitSize() const {
		return magnitude.size() * ValueBits - Bits::clz(magnitude.back());
	}

	constexpr SizeT magnitudeByteSize() const noexcept {
		return magnitude.size() * sizeof(ValueT);
	}


	constexpr std::string toString() const {

		std::string result;

		BigInt b = *this;

		while (!b.isZero()) {

			DivResult digit = b.divmod(10);

			result += char('0') + digit.remainder.toInteger<i8>();
			b = digit.quotient;

		}

		if (result.empty()) {
			return "0";
		}

		std::reverse(result.begin(), result.end());

		if (isNegative()) {
			result = '-' + result;
		}

		return result;

	}


	static BigInt parseString(std::string_view str) {

		BigInt b;

		bool sign = false;
		ValueT value = 0;

		SizeT bufferOps = static_cast<SizeT>(Math::floor(Math::log(std::numeric_limits<ValueT>::max())));
		ValueT factor = static_cast<ValueT>(Math::pow(10, bufferOps));

		SizeT i = 0;

		if (str.size() >= 2 && str[0] == '-') {

			if (!Math::inRange(str[1], '0', '9')) {
				throw std::runtime_error("Bad integer string");
			}

			sign = true;
			str = { str.data() + 1, str.size() - 1 };

		}

		for (char c : str) {

			if (!Math::inRange(c, '0', '9')) {
				throw std::runtime_error("Bad integer string");
			}

			value *= 10;
			value += c - '0';

			i++;

			if (i == bufferOps) {

				b = addCoreUnchecked(multiplyMagnitudeSingle(b, factor), value);
				value = 0;
				i = 0;

			}

		}

		//Add the remaining digits
		if (i) {
			b = addCoreUnchecked(multiplyMagnitudeSingle(b, static_cast<ValueT>(Math::pow(10, i))), value);
		}

		if (!b.checkZero()) {
			b.signum = sign ? -1 : 1;
		}

		return b;

	}


private:

	constexpr static BigInt addCore(const BigInt& a, const BigInt& b) {

		if (b.isZero()) {
			return a;
		} else if (a.isZero()) {
			return b;
		}

		if (a.isPositive() ^ b.isPositive()) {
			return subtractCoreUnchecked(a, b.negate());
		}

		return addCoreUnchecked(a, b);

	}

	constexpr static BigInt subtractCore(const BigInt& a, const BigInt& b) {

		if (b.isZero()) {
			return a;
		} else if (a.isZero()) {
			return b.negate();
		}

		if (a.isPositive() ^ b.isPositive()) {
			return addCoreUnchecked(a, b.negate());
		}

		if (b > a) {
			return subtractCoreUnchecked(b, a).negate();
		} else if (b == a) {
			return {};
		} else {
			return subtractCoreUnchecked(a, b);
		}

	}

	constexpr static BigInt multiplyCore(const BigInt& a, const BigInt& b) {

		if (a.isZero() || b.isZero()) {
			return {};
		} else if (a == 1) {
			return b;
		} else if (b == 1) {
			return a;
		}

		return multiplyCoreUnchecked(a, b);

	}

	constexpr static DivResult divideCore(const BigInt& a, const BigInt& b) {

		if (b.isZero()) {
			//Throw
			throw 9;
		} else if (b == 1) {
			return {a, 0};
		} else if (a.isZero()) {
			return {};
		}

		return divideCoreUnchecked(a, b);

	}

	constexpr static BigInt powCore(const BigInt& base, const BigInt& exp) {

		//Illegal operation
		if (base.isZero() && exp.isZero()) {

		} else if (base.isZero() || exp.isNegative()) {
			return {};
		} else if (exp.isZero() || base == 1) {
			return 1;
		}

		return powCoreUnchecked(base, exp);

	}

	constexpr static BigInt addCoreUnchecked(BigInt a, const BigInt& b) {

		a.widen(b.magnitude.size());

		SizeT i = 0;
		bool carry = false;

		for (; i < b.magnitude.size(); i++) {

			OverflowAddResult<ValueT> result = Math::overflowAdd(a.magnitude[i], b.magnitude[i]);

			a.magnitude[i] = result.value + carry;
			carry = result.overflown || (result.value == -1 && carry);

		}

		for (; i < a.magnitude.size() && carry; i++) {

			a.magnitude[i]++;
			carry = a.magnitude[i] == 0;

		}

		if (carry) {
			a.magnitude.emplace_back(1);
		}

		a.compress();

		return a;

	}

	constexpr static BigInt subtractCoreUnchecked(BigInt a, const BigInt& b) {

		a.widen(b.magnitude.size());

		SizeT i = 0;
		bool borrow = false;

		for (; i < b.magnitude.size(); i++) {

			OverflowAddResult<ValueT> result = Math::overflowSubtract(a.magnitude[i], b.magnitude[i]);

			a.magnitude[i] = result.value - borrow;
			borrow = result.overflown || (result.value == 0 && borrow);

		}

		for (; i < a.magnitude.size() && borrow; i++) {

			a.magnitude[i]--;
			borrow = a.magnitude[i] == -1;

		}

		arc_assert(!borrow, "Illegal borrow");

		a.compress();

		return a;

	}

	constexpr static BigInt multiplyCoreUnchecked(const BigInt& a, const BigInt& b) {

		BigInt result;
		bool sign = a.isPositive() ^ b.isPositive();

		for (SizeT i = 0; ValueT v : b.magnitude) {
			result = addCore(result, multiplyMagnitudeSingle(a, v).shiftLeft(i++ * ValueBits));
		}

		result.signum = sign ? -1 : 1;

		return result;

	}

	constexpr static BigInt multiplyMagnitudeSingle(BigInt a, ValueT b) {

		bool carry = false;
		ValueT prevTop = 0;

		OverflowMultiplyResult<ValueT> multiplyResult;
		OverflowAddResult<ValueT> addResult;

		for (ValueT& v : a.magnitude) {

			multiplyResult = Math::overflowMultiply(v, b);
			addResult = Math::overflowAdd(multiplyResult.bottom, prevTop);

			v = addResult.value + carry;
			carry = addResult.overflown || (carry && addResult.value == -1);
			prevTop = multiplyResult.top;

		}

		prevTop += carry;

		if (multiplyResult.overflown) {
			a.magnitude.emplace_back(prevTop);
		}

		return a;

	}

	constexpr static DivResult divideCoreUnchecked(BigInt a, BigInt b) {

		BigInt result;

		bool sign = a.isPositive() ^ b.isPositive();
		a = a.abs();
		b = b.abs();

		if (a < b) {
			return {{}, a};
		}

		SizeT bitsA = a.magnitudeBitSize();
		SizeT bitsB = b.magnitudeBitSize();

		SizeT shift = bitsA - bitsB;
		b.shiftLeft(shift);

		//Divisor may be greater than dividend, shift once to correct
		if (a < b) {

			b.shiftRight(1);
			shift--;

		}

		for (SizeT i = 0; i <= shift; i++) {

			result.shiftLeft(1);

			if (a >= b) {

				a.subtract(b);
				result.increment();

			}

			b.shiftRight(1);

		}

		result.signum = sign ? -1 : 1;

		return {result, a};

	}

	constexpr static BigInt powCoreUnchecked(BigInt base, BigInt exp) {

		BigInt result(1);

		while (exp) {

			if (exp.lowestBitSet()) {
				result = multiplyCoreUnchecked(result, base);
			}

			base = multiplyCoreUnchecked(base, base);
			exp.shiftRight(1);

		}

		return result;

	}

	constexpr void incrementMagnitude() {

		bool carry = true;

		for (SizeT i = 0; i < magnitude.size() && carry; i++) {

			magnitude[i]++;
			carry = magnitude[i] == 0;

		}

		if (carry) {
			magnitude.emplace_back(1);
		}

	}

	constexpr void decrementMagnitude() {

		bool borrow = true;

		for (SizeT i = 0; i < magnitude.size() && borrow; i++) {

			borrow = magnitude[i] == 0;
			magnitude[i]--;

		}

		arc_assert(!borrow, "Illegal borrow");

	}

	constexpr void widen(SizeT minSize) {

		if (magnitude.size() < minSize) {
			magnitude.resize(minSize);
		}

	}

	constexpr void compress() {

		SizeT compSize = magnitude.size();

		for (SizeT i = magnitude.size() - 1; i > 0; i--) {

			if (magnitude[i] == 0) {
				compSize--;
			} else {
				break;
			}

		}

		magnitude.resize(compSize);

	}

	constexpr void setZero() {

		signum = 0;
		magnitude.resize(1);
		magnitude.front() = 0;

	}

	constexpr bool checkZero() {

		if (magnitude.size() == 1 && magnitude.front() == 0) {

			signum = 0;
			return true;

		}

		return false;

	}


	i32 signum;
	std::vector<ValueT> magnitude;

};



constexpr BigInt operator+(BigInt a, const BigInt& b) {

	a += b;
	return a;

}

constexpr BigInt operator-(BigInt a, const BigInt& b) {

	a -= b;
	return a;

}

constexpr BigInt operator*(BigInt a, const BigInt& b) {

	a *= b;
	return a;

}

constexpr BigInt operator/(BigInt a, const BigInt& b) {

	a /= b;
	return a;

}

constexpr BigInt operator%(BigInt a, const BigInt& b) {

	a %= b;
	return a;

}