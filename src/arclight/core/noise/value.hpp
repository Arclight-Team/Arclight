/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 value.hpp
 */

#pragma once

#include "noisebase.hpp"


enum class ValueNoiseFlag {
	None,
	Linear,
	Smooth
};


template<NoiseFractal Fractal = NoiseFractal::Standard, ValueNoiseFlag Flag = ValueNoiseFlag::None>
class ValueNoiseBase : public NoiseBase {

public:

	using FlagT = ValueNoiseFlag;


	template<FloatParam T, Arithmetic A, Arithmetic L = u32, Arithmetic P = u32>
	constexpr TT::CommonArithmeticType<T> sample(const T& point, A frequency, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {
		return fractalSample<Fractal>([this](const T& p, A f) constexpr { return raw(p, f); }, point, frequency, octaves, lacunarity, persistence);
	}

	template<FloatParam T, Arithmetic A, Arithmetic L = u32, Arithmetic P = u32, Float F = TT::CommonArithmeticType<T>>
	constexpr std::vector<F> sample(std::span<const T> points, std::span<const A> frequencies, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {
		return fractalSample<Fractal>([this](auto p, auto f) constexpr { return raw(p, f); }, points, frequencies, octaves, lacunarity, persistence);
	}

private:

	template<Float F, Arithmetic A>
	constexpr F raw(F point, A frequency) const {

		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		point *= frequency;

		I ip = Math::floor(point);

		UI hp0 = ip & hashMask;

		auto part = [&](UI hp) constexpr {
			return hash(hp) * hashScale<F>;
		};

		F sample;

		if constexpr (Flag == ValueNoiseFlag::None) {

			sample = part(hp0) * 2 - 1;

		} else {

			UI hp1 = hp0 + 1;

			F p = point - ip;

			sample = Math::lerp(part(hp0), part(hp1), interpolate(p)) * 2 - 1;
		}

		return applyFractal<Fractal>(sample);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 2)
	constexpr typename V::Type raw(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		F x = point.x * frequency;
		F y = point.y * frequency;

		I ipx = Math::floor(x);
		I ipy = Math::floor(y);

		UI hpx0 = ipx & hashMask;
		UI hpy0 = ipy & hashMask;

		auto part = [&](UI hpx, UI hpy) constexpr {
			return hash(hpx, hpy) * hashScale<F>;
		};

		F sample;

		if constexpr (Flag == ValueNoiseFlag::None) {

			sample = part(hpx0, hpy0) * 2 - 1;

		} else {

			UI hpx1 = hpx0 + 1;
			UI hpy1 = hpy0 + 1;

			F px = x - ipx;
			F py = y - ipy;

			F stepx = interpolate(px);

			F sample0y = Math::lerp(part(hpx0, hpy0), part(hpx1, hpy0), stepx);
			F sample1y = Math::lerp(part(hpx0, hpy1), part(hpx1, hpy1), stepx);

			F stepy = interpolate(py);

			sample = Math::lerp(sample0y, sample1y, stepy) * 2 - 1;
		}

		return applyFractal<Fractal>(sample);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 3)
	constexpr typename V::Type raw(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		F x = point.x * frequency;
		F y = point.y * frequency;
		F z = point.z * frequency;

		I ipx = Math::floor(x);
		I ipy = Math::floor(y);
		I ipz = Math::floor(z);

		UI hpx0 = ipx & hashMask;
		UI hpy0 = ipy & hashMask;
		UI hpz0 = ipz & hashMask;

		auto part = [&](UI hpx, UI hpy, UI hpz) constexpr {
			return hash(hpx, hpy, hpz) * hashScale<F>;
		};

		F sample;

		if constexpr (Flag == ValueNoiseFlag::None) {

			sample = part(hpx0, hpy0, hpz0) * 2 - 1;

		} else {

			UI hpx1 = hpx0 + 1;
			UI hpy1 = hpy0 + 1;
			UI hpz1 = hpz0 + 1;

			F px = x - ipx;
			F py = y - ipy;
			F pz = z - ipz;

			F stepx = interpolate(px);

			F sample0x = Math::lerp(part(hpx0, hpy0, hpz0), part(hpx1, hpy0, hpz0), stepx);
			F sample1x = Math::lerp(part(hpx0, hpy0, hpz1), part(hpx1, hpy0, hpz1), stepx);
			F sample2x = Math::lerp(part(hpx0, hpy1, hpz0), part(hpx1, hpy1, hpz0), stepx);
			F sample3x = Math::lerp(part(hpx0, hpy1, hpz1), part(hpx1, hpy1, hpz1), stepx);

			F stepy = interpolate(py);

			F sample0y = Math::lerp(sample0x, sample2x, stepy);
			F sample1y = Math::lerp(sample1x, sample3x, stepy);

			F stepz = interpolate(pz);

			sample = Math::lerp(sample0y, sample1y, stepz) * 2 - 1;
		}

		return applyFractal<Fractal>(sample);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 4)
	constexpr typename V::Type raw(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		F x = point.x * frequency;
		F y = point.y * frequency;
		F z = point.z * frequency;
		F w = point.w * frequency;

		I ipx = Math::floor(x);
		I ipy = Math::floor(y);
		I ipz = Math::floor(z);
		I ipw = Math::floor(w);

		UI hpx0 = ipx & hashMask;
		UI hpy0 = ipy & hashMask;
		UI hpz0 = ipz & hashMask;
		UI hpw0 = ipw & hashMask;

		auto part = [&](UI hpx, UI hpy, UI hpz, UI hpw) constexpr {
			return hash(hpx, hpy, hpz, hpw) * hashScale<F>;
		};

		F sample;

		if constexpr (Flag == ValueNoiseFlag::None) {

			sample = part(hpx0, hpy0, hpz0, hpw0) * 2 - 1;

		} else {

			UI hpx1 = hpx0 + 1;
			UI hpy1 = hpy0 + 1;
			UI hpz1 = hpz0 + 1;
			UI hpw1 = hpw0 + 1;

			F px = x - ipx;
			F py = y - ipy;
			F pz = z - ipz;
			F pw = w - ipw;

			F stepx = interpolate(px);

			F sample0x = Math::lerp(part(hpx0, hpy0, hpz0, hpw0), part(hpx1, hpy0, hpz0, hpw0), stepx);
			F sample1x = Math::lerp(part(hpx0, hpy0, hpz0, hpw1), part(hpx1, hpy0, hpz0, hpw1), stepx);
			F sample2x = Math::lerp(part(hpx0, hpy0, hpz1, hpw0), part(hpx1, hpy0, hpz1, hpw0), stepx);
			F sample3x = Math::lerp(part(hpx0, hpy0, hpz1, hpw1), part(hpx1, hpy0, hpz1, hpw1), stepx);
			F sample4x = Math::lerp(part(hpx0, hpy1, hpz0, hpw0), part(hpx1, hpy1, hpz0, hpw0), stepx);
			F sample5x = Math::lerp(part(hpx0, hpy1, hpz0, hpw1), part(hpx1, hpy1, hpz0, hpw1), stepx);
			F sample6x = Math::lerp(part(hpx0, hpy1, hpz1, hpw0), part(hpx1, hpy1, hpz1, hpw0), stepx);
			F sample7x = Math::lerp(part(hpx0, hpy1, hpz1, hpw1), part(hpx1, hpy1, hpz1, hpw1), stepx);

			F stepy = interpolate(py);

			F sample0y = Math::lerp(sample0x, sample4x, stepy);
			F sample1y = Math::lerp(sample1x, sample5x, stepy);
			F sample2y = Math::lerp(sample2x, sample6x, stepy);
			F sample3y = Math::lerp(sample3x, sample7x, stepy);

			F stepz = interpolate(pz);

			F sample0z = Math::lerp(sample0y, sample2y, stepz);
			F sample1z = Math::lerp(sample1y, sample3y, stepz);

			F stepw = interpolate(pw);

			sample = Math::lerp(sample0z, sample1z, stepw) * 2 - 1;
		}

		return applyFractal<Fractal>(sample);

	}


	template<FloatParam T, Arithmetic A, Float F = TT::CommonArithmeticType<T>>
	constexpr std::vector<F> raw(std::span<const T> points, std::span<const A> frequencies) const {

		std::vector<F> samples;

		for (u32 i = 0; i < points.size(); i++) {
			samples.push_back(raw(points[i], frequencies[i]));
		}

		return samples;
	}


	template<Float F>
	static constexpr F hashScale = 1.0 / 0xFF;

	template<Float F>
	static constexpr F interpolate(F t) {
		if constexpr(Flag == ValueNoiseFlag::Smooth) {
			return t * t * t * (t * (t * 6 - 15) + 10);
		} else {
			return t;
		}
	};

};


using ValueNoise				= ValueNoiseBase<NoiseFractal::Standard,	ValueNoiseFlag::None>;
using ValueNoiseLerp			= ValueNoiseBase<NoiseFractal::Standard,	ValueNoiseFlag::Linear>;
using ValueNoiseSmooth			= ValueNoiseBase<NoiseFractal::Standard,	ValueNoiseFlag::Smooth>;
using ValueNoiseRidged			= ValueNoiseBase<NoiseFractal::Ridged,		ValueNoiseFlag::None>;
using ValueNoiseRidgedLerp		= ValueNoiseBase<NoiseFractal::Ridged,		ValueNoiseFlag::Linear>;
using ValueNoiseRidgedSmooth	= ValueNoiseBase<NoiseFractal::Ridged,		ValueNoiseFlag::Smooth>;
using ValueNoiseRidgedSq		= ValueNoiseBase<NoiseFractal::RidgedSq,	ValueNoiseFlag::None>;
using ValueNoiseRidgedSqLerp	= ValueNoiseBase<NoiseFractal::RidgedSq,	ValueNoiseFlag::Linear>;
using ValueNoiseRidgedSqSmooth	= ValueNoiseBase<NoiseFractal::RidgedSq,	ValueNoiseFlag::Smooth>;
