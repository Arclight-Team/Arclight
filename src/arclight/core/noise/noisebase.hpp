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
#include "common/concepts.hpp"
#include <numeric>
#include <random>
#include <array>


template<class Derived>
class NoiseBase {

public:

	NoiseBase() : p(defaultPermutation) {};


	inline void permutate(u32 seed) {
		p = generate(seed);
	}

	inline void permutate() {
		std::random_device rd;
		permutate(rd());
	}


	template<class T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P> requires(CC::Float<T> || FloatVector<T>)
	constexpr auto sample(const T& point, A frequency, u32 octaves, L lacunarity, P persistence) const -> TT::CommonArithmeticType<T> {

		arc_assert(octaves >= 1, "Octaves count cannot be 0");

		using F = TT::CommonArithmeticType<T>;

		const auto& derived = *static_cast<const Derived*>(this);

		F noise = derived.sample(point, frequency);
		F scale = 1;
		F range = 1;

		for (u32 i = 1; i < octaves; i++) {
			frequency *= lacunarity;
			scale *= persistence;
			range += scale;
			noise += derived.sample(point, frequency) * scale;
		}

		return noise / range;

	}

protected:

	static constexpr u32 grad1DMask = 0x1;
	static constexpr u32 grad2DMask = 0x7;
	static constexpr u32 grad3DMask = 0xF;
	static constexpr u32 grad4DMask = 0x1F;


	static constexpr float grad1D[2] = {
		-1, 1
	};

	static const inline Vec2f grad2D[8] = {
		Vec2f( 1, 1).normalized(), Vec2f( 1, 0),
		Vec2f(-1, 1).normalized(), Vec2f(-1, 0),
		Vec2f( 1,-1).normalized(), Vec2f( 0, 1),
		Vec2f(-1,-1).normalized(), Vec2f( 0,-1)
	};

	static const inline Vec3f grad3D[16] = {
		Vec3f( 1, 1,-1).normalized(), Vec3f(-1, 1, 1).normalized(),
		Vec3f(-1, 1,-1).normalized(), Vec3f( 1,-1, 1).normalized(),
		Vec3f( 1,-1,-1).normalized(), Vec3f(-1,-1, 1).normalized(),
		Vec3f(-1,-1,-1).normalized(), Vec3f( 1, 1, 1).normalized(),
		Vec3f( 1, 1, 0).normalized(), Vec3f( 1, 0, 0),
		Vec3f(-1, 1, 0).normalized(), Vec3f(-1, 0, 0),
		Vec3f( 1,-1, 0).normalized(), Vec3f( 0, 1, 0),
		Vec3f(-1,-1, 0).normalized(), Vec3f( 0, 0, 1)
	};

	static const inline Vec4f grad4D[32] = {
		Vec4f(-1, 1,-1, 0).normalized(), Vec4f(-1, 1,-1,-1).normalized(),
		Vec4f( 1,-1,-1, 0).normalized(), Vec4f( 1,-1,-1,-1).normalized(),
		Vec4f(-1,-1,-1, 0).normalized(), Vec4f(-1,-1,-1,-1).normalized(),
		Vec4f( 1, 1, 0, 0).normalized(), Vec4f( 1, 1, 0,-1).normalized(),
		Vec4f( 1,-1, 0, 0).normalized(), Vec4f( 1,-1, 0,-1).normalized(),
		Vec4f(-1,-1, 0, 0).normalized(), Vec4f(-1,-1, 0,-1).normalized(),
		Vec4f(-1, 1, 1, 0).normalized(), Vec4f(-1, 1, 1,-1).normalized(),
		Vec4f( 1,-1, 1, 0).normalized(), Vec4f( 1,-1, 1,-1).normalized(),
		Vec4f( 1, 1, 1, 0).normalized(), Vec4f( 1, 1, 1,-1).normalized(),
		Vec4f( 1, 1,-1, 1).normalized(), Vec4f(-1,-1, 1, 1).normalized(),
		Vec4f(-1, 1,-1, 1).normalized(), Vec4f( 1, 1, 1, 1).normalized(),
		Vec4f( 1,-1,-1, 1).normalized(), Vec4f(-1, 0, 0, 1),
		Vec4f( 1, 1, 0, 1).normalized(), Vec4f( 0, 0, 1, 1),
		Vec4f(-1, 1, 0, 1).normalized(), Vec4f( 0, 1, 0, 0),
		Vec4f(-1,-1, 0, 1).normalized(), Vec4f( 0, 0, 1, 0),
		Vec4f(-1, 1, 1, 1).normalized(), Vec4f( 0, 1, 0,-1),
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

	static PermutationT generate(u32 seed) {

		PermutationT p;

		std::iota(p.begin(), p.begin() + 256, 0);
		std::mt19937 rng(seed);
		std::shuffle(p.begin(), p.begin() + 256, rng);
		std::copy(p.begin(), p.begin() + 256, p.begin() + 256);

		return p;

	}

	static constexpr u32 defaultSeed = 0;

	inline static const PermutationT defaultPermutation = generate(defaultSeed);

	PermutationT p;
};


template<class T>
concept NoiseType = CC::BaseOf<NoiseBase<T>, T>;
