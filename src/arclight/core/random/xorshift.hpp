/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 xorshift.hpp
 */

#pragma once

#include "seed.hpp"
#include "random.hpp"
#include "common/types.hpp"
#include "meta/typetraits.hpp"
#include "util/bits.hpp"



template<CC::UnsignedType T>
class XorShift : public IRandomNumberGenerator {

public:

	using SeedType = Seed<Bits::bitCount<T>()>;

	constexpr XorShift() noexcept {
		setSeed(TT::HasSmallerType<T> ? Bits::allOnes<TT::SmallerType<T>>() : 0xF);
	}

	constexpr explicit XorShift(SeedType seed) noexcept {
		setSeed(seed);
	}

	constexpr void setSeed(SeedType seed) noexcept {
		x = seed.get<T>(0);
	}

protected:

	T x;

};



class XorShift32 : public XorShift<u32> {

	using Base = XorShift<u32>;
	using Base::Base;
	using Base::x;

public:

	constexpr u32 next() noexcept {

		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;

		return x;

	}

};



class XorShift64 : public XorShift<u64> {

	using Base = XorShift<u64>;
	using Base::Base;
	using Base::x;

public:

	constexpr u64 next() noexcept {

		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;

		return x;

	}

};