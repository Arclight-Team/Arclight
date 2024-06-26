/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 LinearCongruentialGenerator.hpp
 */

#pragma once

#include "Seed.hpp"
#include "Random.hpp"
#include "Util/Bits.hpp"
#include "Meta/TypeTraits.hpp"
#include "Meta/Concepts.hpp"
#include "Math/BigInt.hpp"



//M == 0 disables the modulo operation
template<CC::UnsignedType T, T A, T C, T M>
class LinearCongruentialGenerator : public IRandomNumberGenerator {

public:

	using SeedType = Seed<Bits::bitCount<T>()>;
	using result_type = T;

	constexpr LinearCongruentialGenerator() : LinearCongruentialGenerator(1) {}

	constexpr explicit LinearCongruentialGenerator(SeedType seed) {
		setSeed(seed);
	}

	constexpr void setSeed(SeedType seed) noexcept {
		x = seed.template get<T>(0);
	}

	constexpr T next() noexcept(TT::HasBiggerType<T>) {

		if constexpr (TT::HasBiggerType<T>) {

			using U = TT::BiggerType<T>;

			U n = static_cast<U>(A) * x + C;
			return x = M == 0 ? n : n % M;

		} else {

			BigInt n = BigInt(A) * x + C;
			return x = (M == 0 ? n : n % M).template toInteger<T>();

		}

	}

	constexpr T operator()() noexcept {
		return next();
	}

	constexpr static T min() noexcept {
		return 0;
	}

	constexpr static T max() noexcept {
		return T(-1);
	}

private:

	T x;

};


template<CC::UnsignedType T, T A, T M>
using MultiplicativeCongruentialGenerator = LinearCongruentialGenerator<T, A, 0, M>;

using MinStdLCG = MultiplicativeCongruentialGenerator<u32, 48271, 2147483647>;