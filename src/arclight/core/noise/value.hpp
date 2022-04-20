/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 value.hpp
 */

#pragma once

#include "noisebase.hpp"


class ValueNoiseImpl : public NoiseBase {

public:

	template<Float F, Arithmetic A>
	constexpr F sample(F point, A frequency) const {

		point *= frequency;

		i32 ip = i32(Math::floor(point)) & hashMask;

		constexpr F scale = 1.0 / 0xFF;

		return (hash(ip) * scale) * 2 - 1;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 2)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		point *= frequency;

		i32 ipx = i32(Math::floor(point.x)) & hashMask;
		i32 ipy = i32(Math::floor(point.y)) & hashMask;

		constexpr F scale = 1.0 / 0xFF;

		return (hash(ipx, ipy) * scale) * 2 - 1;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 3)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		point *= frequency;

		i32 ipx = i32(Math::floor(point.x)) & hashMask;
		i32 ipy = i32(Math::floor(point.y)) & hashMask;
		i32 ipz = i32(Math::floor(point.z)) & hashMask;

		constexpr F scale = 1.0 / 0xFF;

		return (hash(ipx, ipy, ipz) * scale) * 2 - 1;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 4)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		point *= frequency;

		i32 ipx = i32(Math::floor(point.x)) & hashMask;
		i32 ipy = i32(Math::floor(point.y)) & hashMask;
		i32 ipz = i32(Math::floor(point.z)) & hashMask;
		i32 ipw = i32(Math::floor(point.w)) & hashMask;

		constexpr F scale = 1.0 / 0xFF;

		return (hash(ipx, ipy, ipz, ipw) * scale) * 2 - 1;

	}

	template<class T, Arithmetic A, Arithmetic L, Arithmetic P> requires(Float<T> || FloatVector<T>)
	constexpr auto sample(const T& point, A frequency, u32 octaves, L lacunarity, P persistence) const -> TT::CommonArithmeticType<T> {
		return fractalSample([this](T p, A f) constexpr { return sample(p, f); }, point, frequency, octaves, lacunarity, persistence);
	}

};

using ValueNoise = ValueNoiseImpl;
