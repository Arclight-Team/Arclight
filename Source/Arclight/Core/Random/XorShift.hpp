/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 XorShift.hpp
 */

#pragma once

#include "Seed.hpp"
#include "Random.hpp"
#include "Common/Types.hpp"
#include "Meta/TypeTraits.hpp"
#include "Util/Bits.hpp"



template<CC::UnsignedType T>
class XorShift : public IRandomNumberGenerator {

public:

	using SeedType = Seed<Bits::bitCount<T>()>;

	constexpr XorShift() noexcept {
		setSeed(SeedType(Bits::ones<T>(Bits::bitCount<T>() / 2)));
	}

	constexpr explicit XorShift(SeedType seed) noexcept {
		setSeed(seed);
	}

	constexpr void setSeed(SeedType seed) noexcept {
		x = seed.template get<T>(0);
	}

	constexpr static T min() noexcept {
		return 0;
	}

	constexpr static T max() noexcept {
		return T(-1);
	}

protected:

	T x;

};



class XorShift32 : public XorShift<u32> {

	using Base = XorShift<u32>;
	using Base::Base;
	using Base::x;

public:

	using result_type = u32;

	constexpr u32 next() noexcept {

		x ^= x << 13;
		x ^= x >> 17;
		x ^= x << 5;

		return x;

	}

	constexpr u32 operator()() noexcept {
		return next();
	}

};



class XorShift64 : public XorShift<u64> {

	using Base = XorShift<u64>;
	using Base::Base;
	using Base::x;

public:

	using result_type = u64;

	constexpr u64 next() noexcept {

		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;

		return x;

	}

	constexpr u64 operator()() noexcept {
		return next();
	}

};