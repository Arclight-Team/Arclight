/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 perlin.hpp
 */

#pragma once

#include "noisebase.hpp"


template<NoiseFractal Fractal = NoiseFractal::Standard>
class PerlinNoiseBase : public NoiseBase {

public:

	template<Float F, Arithmetic A>
	constexpr F sample(F point, A frequency) const {

		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		point *= frequency;

		I ip0 = Math::floor(point);

		F p0 = point - ip0;
		F p1 = p0 - 1;

		ip0 &= hashMask;

		UI ip1 = ip0 + 1;

		auto dot = [&](F p, UI ip) constexpr {
			return p * gradient<F>[hash(ip) & grad1DMask];
		};

		constexpr F scale = 2; // sqrt(4/1)

		F sample = Math::lerp(dot(p0, ip0), dot(p1, ip1), interpolate(p0)) * scale;

		return applyFractal<Fractal>(sample);

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 2)
	constexpr typename V::Type sample(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

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

		UI ipx1 = ipx0 + 1;
		UI ipy1 = ipy0 + 1;

		auto dot = [&](F px, F py, UI ipx, UI ipy) constexpr {
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

	template<FloatVector V, Arithmetic A> requires(V::Size == 3)
	constexpr typename V::Type sample(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

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

		UI ipx1 = ipx0 + 1;
		UI ipy1 = ipy0 + 1;
		UI ipz1 = ipz0 + 1;

		auto dot = [&](F px, F py, F pz, UI ipx, UI ipy, UI ipz) constexpr {
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

	template<FloatVector V, Arithmetic A> requires(V::Size == 4)
	constexpr typename V::Type sample(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

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

		UI ipx1 = ipx0 + 1;
		UI ipy1 = ipy0 + 1;
		UI ipz1 = ipz0 + 1;
		UI ipw1 = ipw0 + 1;

		auto dot = [&](F px, F py, F pz, F pw, UI ipx, UI ipy, UI ipz, UI ipw) constexpr {
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

	template<class T, Arithmetic A, Arithmetic L, Arithmetic P> requires(Float<T> || FloatVector<T>)
	constexpr auto sample(const T& point, A frequency, u32 octaves, L lacunarity, P persistence) const -> TT::CommonArithmeticType<T> {
		return fractalSample<Fractal>([this](T p, A f) constexpr { return sample(p, f); }, point, frequency, octaves, lacunarity, persistence);
	}

private:

	template<class T> requires(Float<T> || FloatVector<T>)
	static constexpr T interpolate(const T& t) {
		return t * t * t * (t * (t * T(6) - T(15)) + T(10));
	}

};


using PerlinNoise			= PerlinNoiseBase<NoiseFractal::Standard>;
using PerlinNoiseRidged		= PerlinNoiseBase<NoiseFractal::Ridged>;
using PerlinNoiseRidgedSq	= PerlinNoiseBase<NoiseFractal::RidgedSq>;
