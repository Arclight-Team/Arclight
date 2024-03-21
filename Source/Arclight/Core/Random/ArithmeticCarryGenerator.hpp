/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ArithmeticCarryGenerator.hpp
 */

#pragma once

#include "Seed.hpp"
#include "Random.hpp"
#include "Util/Bits.hpp"
#include "Meta/TypeTraits.hpp"
#include "Meta/Concepts.hpp"
#include "Math/Overflow.hpp"



/*
 * 	Methods for ArithmeticCarryGenerator
 * 	0 = Add-with-carry
 * 	1 = Complementary Add-with-carry
 * 	2 = Subtract-with-carry
 * 	3 = Reversed Subtract-with-carry
 */
template<u32 Method, CC::UnsignedType T, T M, SizeT S, SizeT R>
	requires (Method < 4 && 0 < S < R)
class ArithmeticCarryGenerator : public IRandomNumberGenerator {

public:

	using SeedType = Seed<R * Bits::bitCount<T>() + 1>;

	constexpr static bool Reduce = M != 0;


	constexpr ArithmeticCarryGenerator() noexcept : ArithmeticCarryGenerator(19780503) {}

	constexpr explicit ArithmeticCarryGenerator(SeedType seed) noexcept {
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

		T k;

		if constexpr (Reduce) {

			if constexpr (Method < 2) {

				k = x[index - S] + x[index - R] + carry;

				if (k >= M) {

					k -= M;
					carry = true;

				} else {

					carry = false;

				}

			} else {

				T a, b;

				if constexpr (Method == 2) {

					a = x[index - S];
					b = x[index - R] + carry;

				} else {

					a = x[index - R];
					b = x[index - S] + carry;

				}

				k = a - b;

				if (a < b) {

					k += M;
					carry = true;

				} else {

					carry = false;

				}

			}

		} else {

			T a, b;

			if constexpr (Method <= 2) {

				a = x[index - S];
				b = x[index - R];

			} else {

				a = x[index - R];
				b = x[index - S];

			}

			OverflowAddResult<T> r0, r1;

			if constexpr (Method < 2) {

				r0 = Math::overflowAdd<T>(a, b);
				r1 = Math::overflowAdd<T>(r0.value, carry);

			} else {

				r0 = Math::overflowSubtract<T>(a, b);
				r1 = Math::overflowSubtract<T>(r0.value, carry);

			}

			k = r1.value;
			carry = r0.overflown | r1.overflown;

		}

		x[index - R] = k;
		x[index] = k;
		index++;

		if constexpr (Method == 1) {
			return M - 1 - k;
		} else {
			return k;
		}

	}

private:

	std::array<T, R * 2> x;
	bool carry;
	SizeT index;

};


template<CC::UnsignedType T, T M, SizeT S, SizeT R> using AddWithCarryGenerator 				= ArithmeticCarryGenerator<0, T, M, S, R>;
template<CC::UnsignedType T, T M, SizeT S, SizeT R> using ComplementaryAddWithCarryGenerator 	= ArithmeticCarryGenerator<1, T, M, S, R>;
template<CC::UnsignedType T, T M, SizeT S, SizeT R> using SubtractWithCarryGenerator 			= ArithmeticCarryGenerator<2, T, M, S, R>;
template<CC::UnsignedType T, T M, SizeT S, SizeT R> using ReversedSubtractWithCarryGenerator 	= ArithmeticCarryGenerator<3, T, M, S, R>;

using Ranlux24 = SubtractWithCarryGenerator<u32, 24, 10, 24>;
using Ranlux48 = SubtractWithCarryGenerator<u64, 48, 5, 12>;