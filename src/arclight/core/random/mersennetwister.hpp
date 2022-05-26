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
#include "types.hpp"
#include "util/bits.hpp"
#include "common/concepts.hpp"

#include <array>



template<CC::UnsignedType X, SizeT W, SizeT N, SizeT M, SizeT R, X A, SizeT U, X D, SizeT S, X B, SizeT T, X C, SizeT L, X F>
	requires  (Bits::bitCount<X>() >= W
			&& 2 < W
			&& 1 <= M < N
			&& 0 <= R <= W - 1
			&& S < W
			&& T < W
			&& U < W
			&& L < W
			&& A <= Bits::ones<X>(W)
			&& B <= Bits::ones<X>(W)
			&& C <= Bits::ones<X>(W)
			&& D <= Bits::ones<X>(W)
			&& F <= Bits::ones<X>(W))
class MersenneTwister : public IRandomNumberGenerator {

public:

	using SeedType = Seed<Bits::bitCount<X>()>;

	constexpr static X LMask = Bits::ones<X>(R);
	constexpr static X WMask = Bits::ones<X>(W);
	constexpr static X UMask = ~LMask & WMask;


	constexpr MersenneTwister() : MersenneTwister(5489) {}

	constexpr explicit MersenneTwister(SeedType seed) {
		setSeed(seed);
	}

	constexpr void setSeed(SeedType seed) {

		x[0] = seed.get<X>(0);

		for (SizeT i = 1; i < N; i++) {

			X p = x[i - 1];
			x[i] = (F * (p ^ (p >> (W - 2))) + i) & WMask;

		}

		index = N;

	}

	constexpr X next() noexcept {

		if (index == N) {

			twistGenerator();
			index = 0;

		}

		X y = x[index++];
		y ^= ((y >> U) & D);
		y ^= ((y << S) & B);
		y ^= ((y << T) & C);
		y ^= y >> L;

		return y & WMask;

	}

private:

	constexpr void twistGenerator() noexcept {

		constexpr X t[2] = {0, A};

		SizeT i = 0;

		for (; i < N - M; i++) {

			X y = x[i] & UMask | x[i + 1] & LMask;
			x[i] = x[i + M] ^ (y >> 1) ^ t[y & 1];

		}

		for (; i < N - 1; i++) {

			X y = x[i] & UMask | x[i + 1] & LMask;
			x[i] = x[i + M - N] ^ (y >> 1) ^ t[y & 1];

		}

		X y = x[N - 1] & UMask | x[0] & LMask;
		x[N - 1] = x[M - 1] ^ (y >> 1) ^ t[y & 1];

	}

	std::array<X, N> x;
	SizeT index;

};


using Mt19937 = MersenneTwister<u32, 32, 624, 397, 31, 0x9908B0DF, 11, 0xFFFFFFFF, 7, 0x9D2C5680, 15, 0xEFC60000, 18, 0x6C078965>;
using Mt19937_64 = MersenneTwister<u64, 64, 312, 156, 31, 0xB5026F5AA96619E9, 29, 0x5555555555555555, 17, 0x71D67FFFEDA60000, 37, 0xFFF7EEE000000000, 43, 0x5851F42D4C957F2D>;