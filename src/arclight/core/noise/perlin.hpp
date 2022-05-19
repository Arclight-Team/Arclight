/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 perlin.hpp
 */

#pragma once

#include "noisebase.hpp"


template<NoiseFractal Fractal>
class PerlinNoiseBase : public NoiseBase {

public:

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L = u32, CC::Arithmetic P = u32>
	constexpr TT::CommonArithmeticType<T> sample(const T& point, A frequency, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {
		return fractalSample<Fractal>([this](const T& p, A f) constexpr { return raw(p, f); }, point, frequency, octaves, lacunarity, persistence);
	}

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L = u32, CC::Arithmetic P = u32, CC::Float F = TT::CommonArithmeticType<T>>
	constexpr std::vector<F> sample(std::span<const T> points, std::span<const A> frequencies, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {
		return fractalSample<Fractal>([this](auto p, auto f) constexpr { return raw(p, f); }, points, frequencies, octaves, lacunarity, persistence);
	}

private:

	template<CC::Float F, CC::Arithmetic A>
	constexpr F raw(F point, A frequency) const {

		using I = TT::ToInteger<F>;

		point *= frequency;

		I ip0 = Math::floor(point);

		F p0 = point - ip0;
		F p1 = p0 - 1;

		ip0 &= hashMask;

		u32 ip1 = ip0 + 1;

		auto dot = [&](F p, u32 ip) constexpr {
			return p * gradient<F>[hash(ip) & grad1DMask];
		};

		constexpr F scale = 2; // sqrt(4/1)

		F sample = Math::lerp(dot(p0, ip0), dot(p1, ip1), interpolate(p0)) * scale;

		return applyFractal<Fractal>(sample);

	}

	template<CC::FloatVector V, CC::Arithmetic A> requires(V::Size == 2)
	constexpr typename V::Type raw(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;

		F x = point.x * frequency;
		F y = point.y * frequency;

		I ipx0 = Math::floor(x);
		I ipy0 = Math::floor(y);

		F px0 = x - ipx0;
		F py0 = y - ipy0;
		F px1 = px0 - 1;
		F py1 = py0 - 1;

		ipx0 &= hashMask;
		ipy0 &= hashMask;

		u32 ipx1 = ipx0 + 1;
		u32 ipy1 = ipy0 + 1;

		auto dot = [&](F px, F py, u32 ipx, u32 ipy) constexpr {
			const auto& [gx, gy] = gradient<V>[hash(ipx, ipy) & grad2DMask];
			return px * gx + py * gy;
		};

		F stepx = interpolate(px0);

		F sample0y = Math::lerp(dot(px0, py0, ipx0, ipy0), dot(px1, py0, ipx1, ipy0), stepx);
		F sample1y = Math::lerp(dot(px0, py1, ipx0, ipy1), dot(px1, py1, ipx1, ipy1), stepx);

		F stepy = interpolate(py0);

		constexpr F scale = 1.41421356237; // sqrt(4/2)

		F sample = Math::lerp(sample0y, sample1y, stepy) * scale;

		return applyFractal<Fractal>(sample);

	}

	template<CC::FloatVector V, CC::Arithmetic A> requires(V::Size == 3)
	constexpr typename V::Type raw(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;

		F x = point.x * frequency;
		F y = point.y * frequency;
		F z = point.z * frequency;

		I ipx0 = Math::floor(x);
		I ipy0 = Math::floor(y);
		I ipz0 = Math::floor(z);

		F px0 = x - ipx0;
		F py0 = y - ipy0;
		F pz0 = z - ipz0;
		F px1 = px0 - 1;
		F py1 = py0 - 1;
		F pz1 = pz0 - 1;

		ipx0 &= hashMask;
		ipy0 &= hashMask;
		ipz0 &= hashMask;

		u32 ipx1 = ipx0 + 1;
		u32 ipy1 = ipy0 + 1;
		u32 ipz1 = ipz0 + 1;

		auto dot = [&](F px, F py, F pz, u32 ipx, u32 ipy, u32 ipz) constexpr {
			const auto& [gx, gy, gz] = gradient<V>[hash(ipx, ipy, ipz) & grad3DMask];
			return px * gx + py * gy + pz * gz;
		};

		F stepx = interpolate(px0);

		F sample0x = Math::lerp(dot(px0, py0, pz0, ipx0, ipy0, ipz0), dot(px1, py0, pz0, ipx1, ipy0, ipz0), stepx);
		F sample1x = Math::lerp(dot(px0, py0, pz1, ipx0, ipy0, ipz1), dot(px1, py0, pz1, ipx1, ipy0, ipz1), stepx);
		F sample2x = Math::lerp(dot(px0, py1, pz0, ipx0, ipy1, ipz0), dot(px1, py1, pz0, ipx1, ipy1, ipz0), stepx);
		F sample3x = Math::lerp(dot(px0, py1, pz1, ipx0, ipy1, ipz1), dot(px1, py1, pz1, ipx1, ipy1, ipz1), stepx);

		F stepy = interpolate(py0);

		F sample0y = Math::lerp(sample0x, sample2x, stepy);
		F sample1y = Math::lerp(sample1x, sample3x, stepy);

		F stepz = interpolate(pz0);

		constexpr F scale = 1.15470053838; // sqrt(4/3)

		F sample = Math::lerp(sample0y, sample1y, stepz) * scale;

		return applyFractal<Fractal>(sample);

	}

	template<CC::FloatVector V, CC::Arithmetic A> requires(V::Size == 4)
	constexpr typename V::Type raw(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;

		F x = point.x * frequency;
		F y = point.y * frequency;
		F z = point.z * frequency;
		F w = point.w * frequency;

		I ipx0 = Math::floor(x);
		I ipy0 = Math::floor(y);
		I ipz0 = Math::floor(z);
		I ipw0 = Math::floor(w);

		F px0 = x - ipx0;
		F py0 = y - ipy0;
		F pz0 = z - ipz0;
		F pw0 = w - ipw0;
		F px1 = px0 - 1;
		F py1 = py0 - 1;
		F pz1 = pz0 - 1;
		F pw1 = pw0 - 1;

		ipx0 &= hashMask;
		ipy0 &= hashMask;
		ipz0 &= hashMask;
		ipw0 &= hashMask;

		u32 ipx1 = ipx0 + 1;
		u32 ipy1 = ipy0 + 1;
		u32 ipz1 = ipz0 + 1;
		u32 ipw1 = ipw0 + 1;

		auto dot = [&](F px, F py, F pz, F pw, u32 ipx, u32 ipy, u32 ipz, u32 ipw) constexpr {
			const auto& [gx, gy, gz, gw] = gradient<V>[hash(ipx, ipy, ipz, ipw) & grad4DMask];
			return px * gx + py * gy + pz * gz + pw * gw;
		};

		F stepx = interpolate(px0);

		F sample0x = Math::lerp(dot(px0, py0, pz0, pw0, ipx0, ipy0, ipz0, ipw0), dot(px1, py0, pz0, pw0, ipx1, ipy0, ipz0, ipw0), stepx);
		F sample1x = Math::lerp(dot(px0, py0, pz0, pw1, ipx0, ipy0, ipz0, ipw1), dot(px1, py0, pz0, pw1, ipx1, ipy0, ipz0, ipw1), stepx);
		F sample2x = Math::lerp(dot(px0, py0, pz1, pw0, ipx0, ipy0, ipz1, ipw0), dot(px1, py0, pz1, pw0, ipx1, ipy0, ipz1, ipw0), stepx);
		F sample3x = Math::lerp(dot(px0, py0, pz1, pw1, ipx0, ipy0, ipz1, ipw1), dot(px1, py0, pz1, pw1, ipx1, ipy0, ipz1, ipw1), stepx);
		F sample4x = Math::lerp(dot(px0, py1, pz0, pw0, ipx0, ipy1, ipz0, ipw0), dot(px1, py1, pz0, pw0, ipx1, ipy1, ipz0, ipw0), stepx);
		F sample5x = Math::lerp(dot(px0, py1, pz0, pw1, ipx0, ipy1, ipz0, ipw1), dot(px1, py1, pz0, pw1, ipx1, ipy1, ipz0, ipw1), stepx);
		F sample6x = Math::lerp(dot(px0, py1, pz1, pw0, ipx0, ipy1, ipz1, ipw0), dot(px1, py1, pz1, pw0, ipx1, ipy1, ipz1, ipw0), stepx);
		F sample7x = Math::lerp(dot(px0, py1, pz1, pw1, ipx0, ipy1, ipz1, ipw1), dot(px1, py1, pz1, pw1, ipx1, ipy1, ipz1, ipw1), stepx);

		F stepy = interpolate(py0);

		F sample0y = Math::lerp(sample0x, sample4x, stepy);
		F sample1y = Math::lerp(sample1x, sample5x, stepy);
		F sample2y = Math::lerp(sample2x, sample6x, stepy);
		F sample3y = Math::lerp(sample3x, sample7x, stepy);

		F stepz = interpolate(pz0);

		F sample0z = Math::lerp(sample0y, sample2y, stepz);
		F sample1z = Math::lerp(sample1y, sample3y, stepz);

		F stepw = interpolate(pw0);

		constexpr F scale = 1; // sqrt(4/4)

		F sample = Math::lerp(sample0z, sample1z, stepw) * scale;

		return applyFractal<Fractal>(sample);

	}


	template<CC::FloatParam T, CC::Arithmetic A, CC::Float F = TT::CommonArithmeticType<T>>
	constexpr std::vector<F> raw(std::span<const T> points, std::span<const A> frequencies) const {

		std::vector<F> samples;

		for (u32 i = 0; i < points.size(); i++) {
			samples.push_back(raw(points[i], frequencies[i]));
		}

		return samples;
	}


	template<CC::Float F>
	static constexpr F interpolate(F t) {
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

};


using PerlinNoise			= PerlinNoiseBase<NoiseFractal::Standard>;
using PerlinNoiseRidged		= PerlinNoiseBase<NoiseFractal::Ridged>;
using PerlinNoiseRidgedSq	= PerlinNoiseBase<NoiseFractal::RidgedSq>;
