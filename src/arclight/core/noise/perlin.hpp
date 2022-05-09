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


	template<CC::Float F, CC::Arithmetic A>
	constexpr F sample(F point, A frequency) const {

		point *= frequency;

		i32 ip0 = Math::floor(point);

		F p0 = point - ip0;
		F p1 = p0 - 1;

		ip0 &= hashMask;

		u32 ip1 = ip0 + 1;

		auto dot = [&](F p, u32 ip) {
			return grad1D[hash(ip) & grad1DMask] * p;
		};

		constexpr F scale = 2; // sqrt(4/1)

		return Math::lerp(dot(p0, ip0), dot(p1, ip1), interpolate(p0)) * scale;

	}

	template<FloatVector V, CC::Arithmetic A> requires(V::Size == 2)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		point *= frequency;

		F x = point.x;
		F y = point.y;

		i32 ipx0 = Math::floor(x);
		i32 ipy0 = Math::floor(y);

		F px0 = x - ipx0;
		F py0 = y - ipy0;
		F px1 = px0 - 1;
		F py1 = py0 - 1;

		ipx0 &= hashMask;
		ipy0 &= hashMask;

		u32 ipx1 = ipx0 + 1;
		u32 ipy1 = ipy0 + 1;

		auto dot = [&](F px, F py, u32 ipx, u32 ipy) {
			return V(px, py).dot(grad2D[hash(ipx, ipy) & grad2DMask]);
		};

		F stepy = interpolate(py0);

		F sample0y = Math::lerp(dot(px0, py0, ipx0, ipy0), dot(px0, py1, ipx0, ipy1), stepy);
		F sample1y = Math::lerp(dot(px1, py0, ipx1, ipy0), dot(px1, py1, ipx1, ipy1), stepy);

		constexpr F scale = 1.41421356237; // sqrt(4/2)

		return Math::lerp(sample0y, sample1y, interpolate(px0)) * scale;

	}

	template<FloatVector V, CC::Arithmetic A> requires(V::Size == 3)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		point *= frequency;

		F x = point.x;
		F y = point.y;
		F z = point.z;

		i32 ipx0 = Math::floor(x);
		i32 ipy0 = Math::floor(y);
		i32 ipz0 = Math::floor(z);

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

		auto dot = [&](F px, F py, F pz, u32 ipx, u32 ipy, u32 ipz) {
			return V(px, py, pz).dot(grad3D[hash(ipx, ipy, ipz) & grad3DMask]);
		};

		F stepz = interpolate(pz0);

		F sample0z = Math::lerp(dot(px0, py0, pz0, ipx0, ipy0, ipz0), dot(px0, py0, pz1, ipx0, ipy0, ipz1), stepz);
		F sample1z = Math::lerp(dot(px0, py1, pz0, ipx0, ipy1, ipz0), dot(px0, py1, pz1, ipx0, ipy1, ipz1), stepz);
		F sample2z = Math::lerp(dot(px1, py0, pz0, ipx1, ipy0, ipz0), dot(px1, py0, pz1, ipx1, ipy0, ipz1), stepz);
		F sample3z = Math::lerp(dot(px1, py1, pz0, ipx1, ipy1, ipz0), dot(px1, py1, pz1, ipx1, ipy1, ipz1), stepz);

		F stepy = interpolate(py0);

		F sample0y = Math::lerp(sample0z, sample1z, stepy);
		F sample1y = Math::lerp(sample2z, sample3z, stepy);

		constexpr F scale = 1.15470053838; // sqrt(4/3)

		return Math::lerp(sample0y, sample1y, interpolate(px0)) * scale;

	}

	template<FloatVector V, CC::Arithmetic A> requires(V::Size == 4)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		point *= frequency;

		F x = point.x;
		F y = point.y;
		F z = point.z;
		F w = point.w;

		i32 ipx0 = Math::floor(x);
		i32 ipy0 = Math::floor(y);
		i32 ipz0 = Math::floor(z);
		i32 ipw0 = Math::floor(w);

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

		auto dot = [&](F px, F py, F pz, F pw, u32 ipx, u32 ipy, u32 ipz, u32 ipw) {
			return V(px, py, pz, pw).dot(grad4D[hash(ipx, ipy, ipz, ipw) & grad4DMask]);
		};

		F stepw = interpolate(pw0);

		F sample0w = Math::lerp(dot(px0, py0, pz0, pw0, ipx0, ipy0, ipz0, ipw0), dot(px0, py0, pz0, pw1, ipx0, ipy0, ipz0, ipw1), stepw);
		F sample1w = Math::lerp(dot(px0, py0, pz1, pw0, ipx0, ipy0, ipz1, ipw0), dot(px0, py0, pz1, pw1, ipx0, ipy0, ipz1, ipw1), stepw);
		F sample2w = Math::lerp(dot(px0, py1, pz0, pw0, ipx0, ipy1, ipz0, ipw0), dot(px0, py1, pz0, pw1, ipx0, ipy1, ipz0, ipw1), stepw);
		F sample3w = Math::lerp(dot(px0, py1, pz1, pw0, ipx0, ipy1, ipz1, ipw0), dot(px0, py1, pz1, pw1, ipx0, ipy1, ipz1, ipw1), stepw);
		F sample4w = Math::lerp(dot(px1, py0, pz0, pw0, ipx1, ipy0, ipz0, ipw0), dot(px1, py0, pz0, pw1, ipx1, ipy0, ipz0, ipw1), stepw);
		F sample5w = Math::lerp(dot(px1, py0, pz1, pw0, ipx1, ipy0, ipz1, ipw0), dot(px1, py0, pz1, pw1, ipx1, ipy0, ipz1, ipw1), stepw);
		F sample6w = Math::lerp(dot(px1, py1, pz0, pw0, ipx1, ipy1, ipz0, ipw0), dot(px1, py1, pz0, pw1, ipx1, ipy1, ipz0, ipw1), stepw);
		F sample7w = Math::lerp(dot(px1, py1, pz1, pw0, ipx1, ipy1, ipz1, ipw0), dot(px1, py1, pz1, pw1, ipx1, ipy1, ipz1, ipw1), stepw);

		F stepz = interpolate(pz0);

		F sample0z = Math::lerp(sample0w, sample1w, stepz);
		F sample1z = Math::lerp(sample2w, sample3w, stepz);
		F sample2z = Math::lerp(sample4w, sample5w, stepz);
		F sample3z = Math::lerp(sample6w, sample7w, stepz);

		F stepy = interpolate(py0);

		F sample0y = Math::lerp(sample0z, sample1z, stepy);
		F sample1y = Math::lerp(sample2z, sample3z, stepy);

		constexpr F scale = 1; // sqrt(4/4)

		return Math::lerp(sample0y, sample1y, interpolate(px0)) * scale;

	}

private:

	template<class T> requires(CC::Float<T> || FloatVector<T>)
	static constexpr T interpolate(const T& t) {
		return t * t * t * (t * (t * T(6) - T(15)) + T(10));
	}

};
