/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 perlin.hpp
 */

#pragma once

#include "noisebase.hpp"


class PerlinNoise : public NoiseBase<PerlinNoise> {

public:

	using NoiseBase<PerlinNoise>::sample;


	template<Float F, Arithmetic A>
	F sample(F point, A frequency) const {

		point *= frequency;

		i32 ip = Math::floor(point);

		F p = point - ip;

		ip &= hashMask;

		auto dot = [&](F offset) {
			return grad1D[hash(ip + offset) & grad1DMask] * (p - offset);
		};

		F step = interpolate(p);

		return Math::lerp(dot(0), dot(1), step);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 2)
	typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;
		using Vi = Vec2i;
		using Vui = Vec2ui;

		point *= frequency;

		Vi ip = Math::floor(point);

		V p = point - ip;

		ip &= hashMask;

		auto dot = [&](const Vui& offset) {
			return grad2D[hash(ip + offset) & grad2DMask].dot(p - offset);
		};

		V step = interpolate(p);

		F sample0y = Math::lerp(dot({0, 0}), dot({0, 1}), step.y);
		F sample1y = Math::lerp(dot({1, 0}), dot({1, 1}), step.y);

		return Math::lerp(sample0y, sample1y, step.x);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 3)
	typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;
		using Vi = Vec3i;
		using Vui = Vec3ui;

		point *= frequency;

		Vi ip = Math::floor(point);

		V p = point - ip;

		ip &= hashMask;

		auto dot = [&](const Vui& offset) {
			return grad3D[hash(ip + offset) & grad3DMask].dot(p - offset);
		};

		V step = interpolate(p);

		F sample0z = Math::lerp(dot({0, 0, 0}), dot({0, 0, 1}), step.z);
		F sample1z = Math::lerp(dot({0, 1, 0}), dot({0, 1, 1}), step.z);
		F sample2z = Math::lerp(dot({1, 0, 0}), dot({1, 0, 1}), step.z);
		F sample3z = Math::lerp(dot({1, 1, 0}), dot({1, 1, 1}), step.z);

		F sample0y = Math::lerp(sample0z, sample1z, step.y);
		F sample1y = Math::lerp(sample2z, sample3z, step.y);

		return Math::lerp(sample0y, sample1y, step.x);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 4)
	typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;
		using Vi = Vec4i;
		using Vui = Vec4ui;

		point *= frequency;

		Vi ip = Math::floor(point);

		V p = point - ip;

		ip &= hashMask;

		auto dot = [&](const Vui& offset) {
			return grad4D[hash(ip + offset) & grad4DMask].dot(p - offset);
		};

		V step = interpolate(p);

		F sample0w = Math::lerp(dot({0, 0, 0, 0}), dot({0, 0, 0, 1}), step.w);
		F sample1w = Math::lerp(dot({0, 0, 1, 0}), dot({0, 0, 1, 1}), step.w);
		F sample2w = Math::lerp(dot({0, 1, 0, 0}), dot({0, 1, 0, 1}), step.w);
		F sample3w = Math::lerp(dot({0, 1, 1, 0}), dot({0, 1, 1, 1}), step.w);
		F sample4w = Math::lerp(dot({1, 0, 0, 0}), dot({1, 0, 0, 1}), step.w);
		F sample5w = Math::lerp(dot({1, 0, 1, 0}), dot({1, 0, 1, 1}), step.w);
		F sample6w = Math::lerp(dot({1, 1, 0, 0}), dot({1, 1, 0, 1}), step.w);
		F sample7w = Math::lerp(dot({1, 1, 1, 0}), dot({1, 1, 1, 1}), step.w);

		F sample0z = Math::lerp(sample0w, sample1w, step.z);
		F sample1z = Math::lerp(sample2w, sample3w, step.z);
		F sample2z = Math::lerp(sample4w, sample5w, step.z);
		F sample3z = Math::lerp(sample6w, sample7w, step.z);

		F sample0y = Math::lerp(sample0z, sample1z, step.y);
		F sample1y = Math::lerp(sample2z, sample3z, step.y);

		return Math::lerp(sample0y, sample1y, step.x);

	}

private:

	template<class T> requires(Float<T> || FloatVector<T>)
	static constexpr T interpolate(const T& t) {
		return t * t * t * (t * (t * T(6) - T(15)) + T(10));
	}

};
