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

	template<class T, Arithmetic A, Arithmetic L, Arithmetic P, Invocable<T, A> Func> requires(Float<T> || FloatVector<T>)
	static constexpr auto fractalSample(Func func, const T& point, A frequency, u32 octaves, L lacunarity, P persistence) -> TT::CommonArithmeticType<T> {

		arc_assert(octaves >= 1, "Octaves count cannot be 0");

		using F = TT::CommonArithmeticType<T>;

		F noise = func(point, frequency);
		F scale = 1;
		F range = 1;

		for (u32 i = 1; i < octaves; i++) {
			frequency *= lacunarity;
			scale *= persistence;
			range += scale;
			noise += func(point, frequency) * scale;
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

	constexpr u32 hash(const Vec2ui& vec) const {
		return p[hash(vec.x) + vec.y];
	}

	constexpr u32 hash(const Vec3ui& vec) const {
		return p[hash(vec.toVec2()) + vec.z];
	}

	constexpr u32 hash(const Vec4ui& vec) const {
		return p[hash(vec.toVec3()) + vec.w];
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
