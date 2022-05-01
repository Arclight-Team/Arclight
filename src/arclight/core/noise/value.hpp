/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 value.hpp
 */

#pragma once

#include "noisebase.hpp"


template<NoiseFractal Fractal = NoiseFractal::Standard>
class ValueNoiseBase : public NoiseBase {

public:

	template<Float F, Arithmetic A>
	constexpr F sample(F point, A frequency) const {

		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		point *= frequency;

		UI ip = I(Math::floor(point)) & hashMask;

		constexpr F scale = 1.0 / 0xFF;

		F sample = hash(ip) * scale * 2 - 1;

		return applyFractal<Fractal>(sample);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 2)
	constexpr typename V::Type sample(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		F x = point.x * frequency;
		F y = point.y * frequency;

		UI ipx = I(Math::floor(x)) & hashMask;
		UI ipy = I(Math::floor(y)) & hashMask;

		constexpr F scale = 1.0 / 0xFF;

		F sample = hash(ipx, ipy) * scale * 2 - 1;

		return applyFractal<Fractal>(sample);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 3)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		F x = point.x * frequency;
		F y = point.y * frequency;
		F z = point.z * frequency;

		UI ipx = I(Math::floor(x)) & hashMask;
		UI ipy = I(Math::floor(y)) & hashMask;
		UI ipz = I(Math::floor(z)) & hashMask;

		constexpr F scale = 1.0 / 0xFF;

		F sample = hash(ipx, ipy, ipz) * scale * 2 - 1;

		return applyFractal<Fractal>(sample);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 4)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		F x = point.x * frequency;
		F y = point.y * frequency;
		F z = point.z * frequency;
		F w = point.w * frequency;

		UI ipx = I(Math::floor(x)) & hashMask;
		UI ipy = I(Math::floor(y)) & hashMask;
		UI ipz = I(Math::floor(z)) & hashMask;
		UI ipw = I(Math::floor(w)) & hashMask;

		constexpr F scale = 1.0 / 0xFF;

		F sample = hash(ipx, ipy, ipz, ipw) * scale * 2 - 1;

		return applyFractal<Fractal>(sample);

	}

	template<class T, Arithmetic A, Arithmetic L, Arithmetic P> requires(Float<T> || FloatVector<T>)
	constexpr auto sample(const T& point, A frequency, u32 octaves, L lacunarity, P persistence) const -> TT::CommonArithmeticType<T> {
		return fractalSample<Fractal>([this](T p, A f) constexpr { return sample(p, f); }, point, frequency, octaves, lacunarity, persistence);
	}

};


using ValueNoise			= ValueNoiseBase<NoiseFractal::Standard>;
using ValueNoiseRidged		= ValueNoiseBase<NoiseFractal::Ridged>;
using ValueNoiseRidgedSq	= ValueNoiseBase<NoiseFractal::RidgedSq>;
