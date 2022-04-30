/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 simplex.hpp
 */

#pragma once

#include "noisebase.hpp"


class SimplexNoise : public NoiseBase {

public:

	template<Float F, Arithmetic A>
	F sample(F point, A frequency) const {

		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		point *= frequency;

		I ip0 = Math::floor(point);

		F p0 = point - ip0;
		F p1 = p0 - 1;

		ip0 &= hashMask;

		UI ip1 = ip0 + 1;

		auto part = [&](F p, UI ip) constexpr {

			F dot = p * gradient<F>[hash(ip) & grad1DMask];

			return dot * falloff<F>(1, p);

		};

		F sample = part(p0, ip0) + part(p1, ip1);

		constexpr F scale = 64.0 / 27;

		return sample * scale;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 2)
	typename V::Type sample(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		constexpr F toTriangle = 0.2113248654;
		constexpr F toSquare = 0.36602540378;

		F x = point.x * frequency;
		F y = point.y * frequency;

		F skew = (x + y) * toSquare;
		F skewx = x + skew;
		F skewy = y + skew;

		I ipx0 = Math::floor(skewx);
		I ipy0 = Math::floor(skewy);
		I ipx1 = ipx0 + 1;
		I ipy1 = ipy0 + 1;

		F px0 = x - ipx0;
		F py0 = y - ipy0;
		F px1 = px0 - 1;
		F py1 = py0 - 1;

		UI hpx0 = ipx0 & hashMask;
		UI hpy0 = ipy0 & hashMask;
		UI hpx1 = hpx0 + 1;
		UI hpy1 = hpy0 + 1;

		auto part = [&](F px, F py, I ipx, I ipy, UI hx, UI hy) constexpr {

			F unskew = (ipx + ipy) * toTriangle;
			F unskewx = px + unskew;
			F unskewy = py + unskew;

			const auto& [gx, gy] = gradient<V>[hash(hx, hy) & grad2DMask];
			F dot = unskewx * gx + unskewy * gy;

			return dot * falloff<F>(0.5, unskewx, unskewy);

		};

		F sample = part(px0, py0, ipx0, ipy0, hpx0, hpy0) + part(px1, py1, ipx1, ipy1, hpx1, hpy1);

		F diffx = skewx - ipx0;
		F diffy = skewy - ipy0;

		if (diffx >= diffy) {
			sample += part(px1, py0, ipx1, ipy0, hpx1, hpy0);
		} else {
			sample += part(px0, py1, ipx0, ipy1, hpx0, hpy1);
		}

		constexpr F scale = 32.990773983; // 2916 * sqrt(2) / 125

		return sample * scale;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 3)
	typename V::Type sample(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		constexpr F toTetrahedron = 1.0 / 6;
		constexpr F toCube = 1.0 / 3;

		F x = point.x * frequency;
		F y = point.y * frequency;
		F z = point.z * frequency;

		F skew = (x + y + z) * toCube;
		F skewx = x + skew;
		F skewy = y + skew;
		F skewz = z + skew;

		I ipx0 = Math::floor(skewx);
		I ipy0 = Math::floor(skewy);
		I ipz0 = Math::floor(skewz);
		I ipx1 = ipx0 + 1;
		I ipy1 = ipy0 + 1;
		I ipz1 = ipz0 + 1;

		F px0 = x - ipx0;
		F py0 = y - ipy0;
		F pz0 = z - ipz0;
		F px1 = px0 - 1;
		F py1 = py0 - 1;
		F pz1 = pz0 - 1;

		UI hpx0 = ipx0 & hashMask;
		UI hpy0 = ipy0 & hashMask;
		UI hpz0 = ipz0 & hashMask;
		UI hpx1 = hpx0 + 1;
		UI hpy1 = hpy0 + 1;
		UI hpz1 = hpz0 + 1;

		auto part = [&](F px, F py, F pz, I ipx, I ipy, I ipz, UI hx, UI hy, UI hz) constexpr {

			F unskew = (ipx + ipy + ipz) * toTetrahedron;
			F unskewx = px + unskew;
			F unskewy = py + unskew;
			F unskewz = pz + unskew;

			const auto& [gx, gy, gz] = gradient<V>[hash(hx, hy, hz) & grad3DMask];
			F dot = unskewx * gx + unskewy * gy + unskewz * gz;

			return dot * falloff<F>(0.5, unskewx, unskewy, unskewz);

		};

		F diffx = skewx - ipx0;
		F diffy = skewy - ipy0;
		F diffz = skewz - ipz0;

		bool xy = diffx >= diffy;
		bool xz = diffx >= diffz;
		bool yz = diffy >= diffz;

		F sample1 = part(px0, py0, pz0, ipx0, ipy0, ipz0, hpx0, hpy0, hpz0);
		F sample2 = part(px1, py1, pz1, ipx1, ipy1, ipz1, hpx1, hpy1, hpz1);
		F sample3;
		F sample4;

		if (!xy && yz) {
			sample3 = part(px0, py1, pz0, ipx0, ipy1, ipz0, hpx0, hpy1, hpz0);

			if (xz) {
				sample4 = part(px1, py1, pz0, ipx1, ipy1, ipz0, hpx1, hpy1, hpz0);
			} else {
				sample4 = part(px0, py1, pz1, ipx0, ipy1, ipz1, hpx0, hpy1, hpz1);
			}
		} else if (xz) {
			sample3 = part(px1, py0, pz0, ipx1, ipy0, ipz0, hpx1, hpy0, hpz0);

			if (yz) {
				sample4 = part(px1, py1, pz0, ipx1, ipy1, ipz0, hpx1, hpy1, hpz0);
			} else {
				sample4 = part(px1, py0, pz1, ipx1, ipy0, ipz1, hpx1, hpy0, hpz1);
			}
		} else {
			sample3 = part(px0, py0, pz1, ipx0, ipy0, ipz1, hpx0, hpy0, hpz1);

			if (xy) {
				sample4 = part(px1, py0, pz1, ipx1, ipy0, ipz1, hpx1, hpy0, hpz1);
			} else {
				sample4 = part(px0, py1, pz1, ipx0, ipy1, ipz1, hpx0, hpy1, hpz1);
			}
		}

		constexpr float scale = 30.6822935365; // 8192 * sqrt(3) / (327 * sqrt(2))

		return (sample1 + sample2 + sample3 + sample4) * scale;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 4)
	typename V::Type sample(const V& point, A frequency) const {

		using F = typename V::Type;
		using I = TT::ToInteger<F>;
		using UI = TT::MakeUnsigned<I>;

		constexpr F to5Cell = 0.13819660112;
		constexpr F toTesseract = 0.30901699437;

		F x = point.x * frequency;
		F y = point.y * frequency;
		F z = point.z * frequency;
		F w = point.w * frequency;

		F skew = (x + y + z + w) * toTesseract;
		F skewx = x + skew;
		F skewy = y + skew;
		F skewz = z + skew;
		F skeww = w + skew;

		I ipx0 = Math::floor(skewx);
		I ipy0 = Math::floor(skewy);
		I ipz0 = Math::floor(skewz);
		I ipw0 = Math::floor(skeww);
		I ipx1 = ipx0 + 1;
		I ipy1 = ipy0 + 1;
		I ipz1 = ipz0 + 1;
		I ipw1 = ipw0 + 1;

		F px0 = x - ipx0;
		F py0 = y - ipy0;
		F pz0 = z - ipz0;
		F pw0 = w - ipw0;
		F px1 = px0 - 1;
		F py1 = py0 - 1;
		F pz1 = pz0 - 1;
		F pw1 = pw0 - 1;

		UI hpx0 = ipx0 & hashMask;
		UI hpy0 = ipy0 & hashMask;
		UI hpz0 = ipz0 & hashMask;
		UI hpw0 = ipw0 & hashMask;
		UI hpx1 = hpx0 + 1;
		UI hpy1 = hpy0 + 1;
		UI hpz1 = hpz0 + 1;
		UI hpw1 = hpw0 + 1;

		auto part = [&](F px, F py, F pz, F pw, I ipx, I ipy, I ipz, I ipw, UI hx, UI hy, UI hz, UI hw) constexpr {

			F unskew = (ipx + ipy + ipz + ipw) * to5Cell;
			F unskewx = px + unskew;
			F unskewy = py + unskew;
			F unskewz = pz + unskew;
			F unskeww = pw + unskew;

			const auto& [gx, gy, gz, gw] = gradient<V>[hash(hx, hy, hz, hw) & grad4DMask];
			F dot = unskewx * gx + unskewy * gy + unskewz * gz + unskeww * gw;

			return dot * falloff<F>(0.5, unskewx, unskewy, unskewz, unskeww);

		};

		F diffx = skewx - ipx0;
		F diffy = skewy - ipy0;
		F diffz = skewz - ipz0;
		F diffw = skeww - ipw0;

		bool xy = diffx >= diffy;
		bool xz = diffx >= diffz;
		bool yz = diffy >= diffz;
		bool xw = diffx >= diffw;
		bool yw = diffy >= diffw;
		bool zw = diffz >= diffw;

		F sample1 = part(px0, py0, pz0, pw0, ipx0, ipy0, ipz0, ipw0, hpx0, hpy0, hpz0, hpw0);
		F sample2 = part(px1, py1, pz1, pw1, ipx1, ipy1, ipz1, ipw1, hpx1, hpy1, hpz1, hpw1);
		F sample3;
		F sample4;
		F sample5;

		if (!xy && yw && yz) {
			sample3 = part(px0, py1, pz0, pw0, ipx0, ipy1, ipz0, ipw0, hpx0, hpy1, hpz0, hpw0);

			if (!xw && !zw) {
				sample4 = part(px0, py1, pz0, pw1, ipx0, ipy1, ipz0, ipw1, hpx0, hpy1, hpz0, hpw1);

				if (xz) {
					sample5 = part(px1, py1, pz0, pw1, ipx1, ipy1, ipz0, ipw1, hpx1, hpy1, hpz0, hpw1);
				} else {
					sample5 = part(px0, py1, pz1, pw1, ipx0, ipy1, ipz1, ipw1, hpx0, hpy1, hpz1, hpw1);
				}
			} else if (xz && xw) {
				sample4 = part(px1, py1, pz0, pw0, ipx1, ipy1, ipz0, ipw0, hpx1, hpy1, hpz0, hpw0);

				if (!zw) {
					sample5 = part(px1, py1, pz0, pw1, ipx1, ipy1, ipz0, ipw1, hpx1, hpy1, hpz0, hpw1);
				} else {
					sample5 = part(px1, py1, pz1, pw0, ipx1, ipy1, ipz1, ipw0, hpx1, hpy1, hpz1, hpw0);
				}
			} else {
				sample4 = part(px0, py1, pz1, pw0, ipx0, ipy1, ipz1, ipw0, hpx0, hpy1, hpz1, hpw0);

				if (!xw) {
					sample5 = part(px0, py1, pz1, pw1, ipx0, ipy1, ipz1, ipw1, hpx0, hpy1, hpz1, hpw1);
				} else {
					sample5 = part(px1, py1, pz1, pw0, ipx1, ipy1, ipz1, ipw0, hpx1, hpy1, hpz1, hpw0);
				}
			}
		} else if (!xw && !yw && !zw) {
			sample3 = part(px0, py0, pz0, pw1, ipx0, ipy0, ipz0, ipw1, hpx0, hpy0, hpz0, hpw1);

			if (!xy && yz) {
				sample4 = part(px0, py1, pz0, pw1, ipx0, ipy1, ipz0, ipw1, hpx0, hpy1, hpz0, hpw1);

				if (!xz) {
					sample5 = part(px0, py1, pz1, pw1, ipx0, ipy1, ipz1, ipw1, hpx0, hpy1, hpz1, hpw1);
				} else {
					sample5 = part(px1, py1, pz0, pw1, ipx1, ipy1, ipz0, ipw1, hpx1, hpy1, hpz0, hpw1);
				}
			} else if (xz && xy) {
				sample4 = part(px1, py0, pz0, pw1, ipx1, ipy0, ipz0, ipw1, hpx1, hpy0, hpz0, hpw1);

				if (yz) {
					sample5 = part(px1, py1, pz0, pw1, ipx1, ipy1, ipz0, ipw1, hpx1, hpy1, hpz0, hpw1);
				} else {
					sample5 = part(px1, py0, pz1, pw1, ipx1, ipy0, ipz1, ipw1, hpx1, hpy0, hpz1, hpw1);
				}
			} else {
				sample4 = part(px0, py0, pz1, pw1, ipx0, ipy0, ipz1, ipw1, hpx0, hpy0, hpz1, hpw1);

				if (!xy) {
					sample5 = part(px0, py1, pz1, pw1, ipx0, ipy1, ipz1, ipw1, hpx0, hpy1, hpz1, hpw1);
				} else {
					sample5 = part(px1, py0, pz1, pw1, ipx1, ipy0, ipz1, ipw1, hpx1, hpy0, hpz1, hpw1);
				}
			}
		} else if (!yz && !xz && zw) {
			sample3 = part(px0, py0, pz1, pw0, ipx0, ipy0, ipz1, ipw0, hpx0, hpy0, hpz1, hpw0);

			if (!xy && yw) {
				sample4 = part(px0, py1, pz1, pw0, ipx0, ipy1, ipz1, ipw0, hpx0, hpy1, hpz1, hpw0);

				if (!xw) {
					sample5 = part(px0, py1, pz1, pw1, ipx0, ipy1, ipz1, ipw1, hpx0, hpy1, hpz1, hpw1);
				} else {
					sample5 = part(px1, py1, pz1, pw0, ipx1, ipy1, ipz1, ipw0, hpx1, hpy1, hpz1, hpw0);
				}
			} else if (!xw && !yw) {
				sample4 = part(px0, py0, pz1, pw1, ipx0, ipy0, ipz1, ipw1, hpx0, hpy0, hpz1, hpw1);

				if (!xy) {
					sample5 = part(px0, py1, pz1, pw1, ipx0, ipy1, ipz1, ipw1, hpx0, hpy1, hpz1, hpw1);
				} else {
					sample5 = part(px1, py0, pz1, pw1, ipx1, ipy0, ipz1, ipw1, hpx1, hpy0, hpz1, hpw1);
				}
			} else {
				sample4 = part(px1, py0, pz1, pw0, ipx1, ipy0, ipz1, ipw0, hpx1, hpy0, hpz1, hpw0);

				if (yw) {
					sample5 = part(px1, py1, pz1, pw0, ipx1, ipy1, ipz1, ipw0, hpx1, hpy1, hpz1, hpw0);
				} else {
					sample5 = part(px1, py0, pz1, pw1, ipx1, ipy0, ipz1, ipw1, hpx1, hpy0, hpz1, hpw1);
				}
			}
		} else {
			sample3 = part(px1, py0, pz0, pw0, ipx1, ipy0, ipz0, ipw0, hpx1, hpy0, hpz0, hpw0);

			if (yz && yw) {
				sample4 = part(px1, py1, pz0, pw0, ipx1, ipy1, ipz0, ipw0, hpx1, hpy1, hpz0, hpw0);

				if (!zw) {
					sample5 = part(px1, py1, pz0, pw1, ipx1, ipy1, ipz0, ipw1, hpx1, hpy1, hpz0, hpw1);
				} else {
					sample5 = part(px1, py1, pz1, pw0, ipx1, ipy1, ipz1, ipw0, hpx1, hpy1, hpz1, hpw0);
				}
			} else if (!yz && zw) {
				sample4 = part(px1, py0, pz1, pw0, ipx1, ipy0, ipz1, ipw0, hpx1, hpy0, hpz1, hpw0);

				if (yw) {
					sample5 = part(px1, py1, pz1, pw0, ipx1, ipy1, ipz1, ipw0, hpx1, hpy1, hpz1, hpw0);
				} else {
					sample5 = part(px1, py0, pz1, pw1, ipx1, ipy0, ipz1, ipw1, hpx1, hpy0, hpz1, hpw1);
				}
			} else {
				sample4 = part(px1, py0, pz0, pw1, ipx1, ipy0, ipz0, ipw1, hpx1, hpy0, hpz0, hpw1);

				if (yz) {
					sample5 = part(px1, py1, pz0, pw1, ipx1, ipy1, ipz0, ipw1, hpx1, hpy1, hpz0, hpw1);
				} else {
					sample5 = part(px1, py0, pz1, pw1, ipx1, ipy0, ipz1, ipw1, hpx1, hpy0, hpz1, hpw1);
				}
			}
		}

		constexpr F scale = 27;

		return (sample1 + sample2 + sample3 + sample4 + sample5) * scale;

	}

	template<class T, Arithmetic A, Arithmetic L, Arithmetic P> requires(Float<T> || FloatVector<T>)
	constexpr auto sample(const T& point, A frequency, u32 octaves, L lacunarity, P persistence) const -> TT::CommonArithmeticType<T> {
		return fractalSample([this](T p, A f) constexpr { return sample(p, f); }, point, frequency, octaves, lacunarity, persistence);
	}

private:

	template<Float F, class... Args> requires(TT::IsAllSame<F, Args...>)
	static constexpr F falloff(F a, Args... v) {

		((a -= v * v), ...);

		if (a <= 0) {
			return F(0);
		}

		return a * a * a;
	}

};
