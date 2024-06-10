/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 NoiseBase.hpp
 */

#pragma once

#include "Math/Math.hpp"
#include "Math/Vector.hpp"
#include "Meta/Concepts.hpp"
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

	constexpr NoiseBase() : p(defaultP) {};


	inline void permutate(u32 seed) {
		p = genPermutation(seed);
	}

	inline void permutate() {
		std::random_device rd;
		permutate(rd());
	}

protected:

	template<NoiseFractal Fractal, CC::Float F>
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

	template<NoiseFractal Fractal, CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Invocable<T, A> Func>
	static constexpr TT::ExtractType<T> fractalSample(Func&& func, const T& point, A frequency, u32 octaves, L lacunarity, P persistence) {

		arc_assert(octaves >= 1, "Octaves count cannot be 0");

		using F = TT::ExtractType<T>;

		if (octaves == 1) {
			return func(point, frequency);
		}

		F scale = 1;
		F noise = 0;
		F range = 0;

		for (u32 i = 0; i < octaves; i++) {

			F sample = func(point, frequency);

			noise += sample * scale;

			range += scale;
			frequency *= lacunarity;

			if constexpr (Fractal == NoiseFractal::Standard) {
				scale *= persistence;
			} else {
				scale *= 1 - Math::abs(sample);
				scale *= 0.5;
			}
		}

		return noise / range;

	}

	template<NoiseFractal Fractal, CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, CC::Invocable<T, A> Func, CC::Float F = TT::ExtractType<T>>
	static constexpr std::vector<F> fractalSample(Func&& func, std::span<const T> points, std::span<const A> frequencies, u32 octaves, L lacunarity, P persistence) {

		arc_assert(octaves >= 1, "Octaves count cannot be 0");
		arc_assert(points.size() == frequencies.size(), "The amount of points need to match the amount of frequencies");

		if (octaves == 1) {
			return func(points, frequencies);
		}

		arc_force_assert("Fractal span sampling not yet supported");

		return {};

	}


	static constexpr u32 grad1DMask = 0x1;
	static constexpr u32 grad2DMask = 0x7;
	static constexpr u32 grad3DMask = 0xF;
	static constexpr u32 grad4DMask = 0x1F;


	template<CC::Float F>
	static constexpr F gradient1D[2] = {
		-1, 1
	};

	template<CC::FloatVector V> requires(V::Size == 2)
	static constexpr V gradient2D[8] = {
		{ 0.707107, 0.707107}, { 1, 0},
		{-0.707107, 0.707107}, {-1, 0},
		{ 0.707107,-0.707107}, { 0, 1},
		{-0.707107,-0.707107}, { 0,-1}
	};

	template<CC::FloatVector V> requires(V::Size == 3)
	static constexpr V gradient3D[16] = {
		{ 0.57735, 0.57735,-0.57735}, { 0.707107, 0.707107, 0},
		{-0.57735, 0.57735, 0.57735}, {-0.707107, 0.707107, 0},
		{-0.57735, 0.57735,-0.57735}, { 0.707107,-0.707107, 0},
		{ 0.57735,-0.57735, 0.57735}, {-0.707107,-0.707107, 0},
		{ 0.57735,-0.57735,-0.57735}, { 1, 0, 0},
		{-0.57735,-0.57735, 0.57735}, {-1, 0, 0},
		{-0.57735,-0.57735,-0.57735}, { 0, 1, 0},
		{ 0.57735, 0.57735, 0.57735}, { 0, 0, 1}
	};

	template<CC::FloatVector V> requires(V::Size == 4)
	static constexpr V gradient4D[32] = {
		{-0.5, 0.5,-0.5,-0.5}, {-0.57735, 0.57735,-0.57735, 0},
		{ 0.5,-0.5,-0.5,-0.5}, { 0.57735,-0.57735,-0.57735, 0},
		{-0.5,-0.5,-0.5,-0.5}, {-0.57735,-0.57735,-0.57735, 0},
		{-0.5, 0.5, 0.5,-0.5}, {-0.57735, 0.57735, 0.57735, 0},
		{ 0.5,-0.5, 0.5,-0.5}, { 0.57735,-0.57735, 0.57735, 0},
		{ 0.5, 0.5, 0.5,-0.5}, { 0.57735, 0.57735, 0.57735, 0},
		{ 0.5, 0.5,-0.5, 0.5}, { 0.57735, 0.57735, 0,-0.57735},
		{-0.5,-0.5, 0.5, 0.5}, { 0.57735,-0.57735, 0,-0.57735},
		{-0.5, 0.5,-0.5, 0.5}, {-0.57735,-0.57735, 0,-0.57735},
		{ 0.5, 0.5, 0.5, 0.5}, { 0.57735, 0.57735, 0, 0.57735},
		{ 0.5,-0.5,-0.5, 0.5}, {-0.57735, 0.57735, 0, 0.57735},
		{-0.5, 0.5, 0.5, 0.5}, {-0.57735,-0.57735, 0, 0.57735},
		{ 0.707107, 0.707107, 0, 0},
		{ 0.707107,-0.707107, 0, 0},
		{-0.707107,-0.707107, 0, 0},
		{-1, 0, 0, 1},
		{ 0, 0, 1, 1},
		{ 0, 1, 0, 0},
		{ 0, 0, 1, 0},
		{ 0, 1, 0,-1},
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


namespace CC {

	template<class T>
	concept NoiseType = BaseOf<T, NoiseBase>;

}
