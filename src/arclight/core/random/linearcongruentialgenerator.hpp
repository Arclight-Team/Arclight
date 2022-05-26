/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 mersennetwister.hpp
 */

#pragma once

#include "seed.hpp"
#include "random.hpp"
#include "util/bits.hpp"
#include "common/typetraits.hpp"
#include "common/concepts.hpp"
#include "math/bigint.hpp"



//M == 0 disables the modulo operation
template<CC::UnsignedType T, T A, T C, T M>
class LinearCongruentialGenerator : public IRandomNumberGenerator {

public:

	using SeedType = Seed<Bits::bitCount<T>()>;


	constexpr LinearCongruentialGenerator() : LinearCongruentialGenerator(1) {}

	constexpr explicit LinearCongruentialGenerator(SeedType seed) {
		setSeed(seed);
	}

	constexpr void setSeed(SeedType seed) noexcept {
		x = seed.get<T>(0);
	}

	constexpr T next() noexcept(TT::HasBiggerType<T>) {

		if constexpr (TT::HasBiggerType<T>) {

			using U = TT::BiggerType<T>;

			U n = static_cast<U>(A) * x + C;
			return x = M == 0 ? n : n % M;

		} else {

			BigInt n = BigInt(A) * x + C;
			return x = (M == 0 ? n : n % M).toInteger<T>();

		}

	}

private:

	T x;

};


template<CC::UnsignedType T, T A, T M>
using MultiplicativeCongruentialGenerator = LinearCongruentialGenerator<T, A, 0, M>;

using MinStdLCG = MultiplicativeCongruentialGenerator<u32, 48271, 2147483647>;