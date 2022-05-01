/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 noisebase.hpp
 */

#pragma once

#include "math/math.hpp"
#include "math/vector.hpp"
#include "util/concepts.hpp"
#include <numeric>
#include <random>
#include <array>


enum class NoiseFractal {
	Standard,
	Ridged,
	RidgedSq,
};


class NoiseBase {

public:

	NoiseBase() : p(defaultP) {};


	inline void permutate(u32 seed) {
		p = genPermutation(seed);
	}

	inline void permutate() {
		std::random_device rd;
		permutate(rd());
	}

protected:

	template<NoiseFractal Fractal, Float F>
	static constexpr F applyFractal(F sample) {

		if constexpr (Fractal != NoiseFractal::Standard) {

			sample = 1 - Math::abs(sample);

			if constexpr (Fractal == NoiseFractal::RidgedSq) {
				sample *= sample;
			}

			sample = sample * 2 - 1;
		}

		return sample;

	}

	template<NoiseFractal Fractal, class T, Arithmetic A, Arithmetic L, Arithmetic P, Invocable<T, A> Func> requires(Float<T> || FloatVector<T>)
	static constexpr auto fractalSample(Func func, const T& point, A frequency, u32 octaves, L lacunarity, P persistence) -> TT::CommonArithmeticType<T> {

		arc_assert(octaves >= 1, "Octaves count cannot be 0");

		using F = TT::CommonArithmeticType<T>;

		F scale = 1;
		F noise;
		F range;

		if constexpr (Fractal == NoiseFractal::Standard) {

			noise = func(point, frequency);
			range = 1;

			for (u32 i = 1; i < octaves; i++) {
				frequency *= lacunarity;
				scale *= persistence;
				range += scale;
				noise += func(point, frequency) * scale;
			}

		} else {

			noise = 0;
			range = 0;

			for (u32 i = 0; i < octaves; i++) {

				F sample = func(point, frequency);

				noise += sample * scale;

				frequency *= lacunarity;
				range += scale;

				scale *= 1 - Math::abs(sample);
				scale *= 0.5;

			}
		}

		return noise / range;

	}

	static constexpr u32 grad1DMask = 0x1;
	static constexpr u32 grad2DMask = 0x7;
	static constexpr u32 grad3DMask = 0xF;
	static constexpr u32 grad4DMask = 0x1F;


	template<class T>
	static constexpr T gradient;

	template<Float F>
	static constexpr F gradient<F>[2] = {
		-1, 1
	};

	template<FloatVector V> requires(V::Size == 2)
	static inline const V gradient<V>[8] = {
		V( 1, 1).normalized(), V( 1, 0),
		V(-1, 1).normalized(), V(-1, 0),
		V( 1,-1).normalized(), V( 0, 1),
		V(-1,-1).normalized(), V( 0,-1)
	};

	template<FloatVector V> requires(V::Size == 3)
	static inline const V gradient<V>[16] = {
		V( 1, 1,-1).normalized(), V(-1, 1, 1).normalized(),
		V(-1, 1,-1).normalized(), V( 1,-1, 1).normalized(),
		V( 1,-1,-1).normalized(), V(-1,-1, 1).normalized(),
		V(-1,-1,-1).normalized(), V( 1, 1, 1).normalized(),
		V( 1, 1, 0).normalized(), V( 1, 0, 0),
		V(-1, 1, 0).normalized(), V(-1, 0, 0),
		V( 1,-1, 0).normalized(), V( 0, 1, 0),
		V(-1,-1, 0).normalized(), V( 0, 0, 1)
	};

	template<FloatVector V> requires(V::Size == 4)
	static inline const V gradient<V>[32] = {
		V(-1, 1,-1, 0).normalized(), V(-1, 1,-1,-1).normalized(),
		V( 1,-1,-1, 0).normalized(), V( 1,-1,-1,-1).normalized(),
		V(-1,-1,-1, 0).normalized(), V(-1,-1,-1,-1).normalized(),
		V( 1, 1, 0, 0).normalized(), V( 1, 1, 0,-1).normalized(),
		V( 1,-1, 0, 0).normalized(), V( 1,-1, 0,-1).normalized(),
		V(-1,-1, 0, 0).normalized(), V(-1,-1, 0,-1).normalized(),
		V(-1, 1, 1, 0).normalized(), V(-1, 1, 1,-1).normalized(),
		V( 1,-1, 1, 0).normalized(), V( 1,-1, 1,-1).normalized(),
		V( 1, 1, 1, 0).normalized(), V( 1, 1, 1,-1).normalized(),
		V( 1, 1,-1, 1).normalized(), V(-1,-1, 1, 1).normalized(),
		V(-1, 1,-1, 1).normalized(), V( 1, 1, 1, 1).normalized(),
		V( 1,-1,-1, 1).normalized(), V(-1, 0, 0, 1),
		V( 1, 1, 0, 1).normalized(), V( 0, 0, 1, 1),
		V(-1, 1, 0, 1).normalized(), V( 0, 1, 0, 0),
		V(-1,-1, 0, 1).normalized(), V( 0, 0, 1, 0),
		V(-1, 1, 1, 1).normalized(), V( 0, 1, 0,-1),
	};


	static constexpr u32 hashMask = 0xFF;

	constexpr u32 hash(u32 x) const {
		return p[x];
	}

	constexpr u32 hash(u32 x, u32 y) const {
		return p[hash(x) + y];
	}

	constexpr u32 hash(u32 x, u32 y, u32 z) const {
		return p[hash(x, y) + z];
	}

	constexpr u32 hash(u32 x, u32 y, u32 z, u32 w) const {
		return p[hash(x, y, z) + w];
	}

private:

	using PermutationT = std::array<u32, 512>;

	static PermutationT genPermutation(u32 seed) {

		PermutationT p;

		std::iota(p.begin(), p.begin() + 256, 0);
		std::mt19937 rng(seed);
		std::shuffle(p.begin(), p.begin() + 256, rng);
		std::copy(p.begin(), p.begin() + 256, p.begin() + 256);

		return p;

	}

	static constexpr u32 defaultSeed = 0xA6C;

	static inline const PermutationT defaultP = genPermutation(defaultSeed);

	PermutationT p;
};


template<class T>
concept NoiseType = BaseOf<NoiseBase, T>;
