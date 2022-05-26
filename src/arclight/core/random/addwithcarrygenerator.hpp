/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 addwithcarrygenerator.hpp
 */

#pragma once

#include "seed.hpp"
#include "random.hpp"
#include "util/bits.hpp"
#include "common/typetraits.hpp"
#include "common/concepts.hpp"



template<CC::UnsignedType T, T M, SizeT S, SizeT R, bool Complementary>
	requires (0 < S < R)
class AddWithCarryGenerator : public IRandomNumberGenerator {

public:

	using SeedType = Seed<R * Bits::bitCount<T>() + 1>;

	constexpr static bool Reduce = M != 0;


	constexpr AddWithCarryGenerator() noexcept : AddWithCarryGenerator(0) {}

	constexpr explicit AddWithCarryGenerator(SeedType seed) noexcept {
		setSeed(seed);
	}

	constexpr void setSeed(SeedType seed) noexcept {

		for (SizeT i = 0; i < R; i++) {

			x[i] = seed.get<T>(i * Bits::bitCount<T>());

			if constexpr (Reduce) {
				x[i] %= M;
			}

		}

		carry = seed.get<bool>(R * Bits::bitCount<T>());

		for (SizeT i = R; i < R * 2; i++) {
			x[i] = x[i - R];
		}

		index = R;

	}

	constexpr T next() noexcept {

		if (index == R * 2) {
			index = R;
		}

		if constexpr (Reduce) {

			T k = x[index - S] + x[index - R] + carry;

			if (k >= M) {

				k -= M;
				carry = true;

			} else {

				carry = false;

			}

			x[index - R] = k;
			x[index] = k;

		} else {

			auto [result, c0] = Math::overflowAdd(x[index - S], x[index - R]);
			auto [k, c1] = Math::overflowAdd(result, carry);

			carry = c0 | c1;

			x[index - R] = k;
			x[index] = k;

		}

		if constexpr (Complementary) {
			return M - 1 - x[index++];
		} else {
			return x[index++];
		}

	}

private:

	std::array<T, R * 2> x;
	bool carry;
	SizeT index;

};