/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 worley.hpp
 */

#pragma once

#include "noisebase.hpp"


enum class WorleyNoiseFlag {
	None,
	Second,
	Diff,
};


template<WorleyNoiseFlag Flag = WorleyNoiseFlag::None>
class WorleyNoiseImpl : public NoiseBase {

public:

	using FlagT = WorleyNoiseFlag;


	template<Float F, Arithmetic A>
	constexpr F sample(F point, A frequency) const {

		constexpr F max = 2;

		point *= frequency;

		i32 ipx = Math::floor(point);

		F px = point - ipx;

		F first = max;
		F second = max;

		for (const auto& ofsx : offsets1D) {

			u32 hx = Math::abs(ipx + ofsx) & hashMask;

			F gx = gradient<F>[hash(hx) & grad1DMask];

			gx = gx / 2 + 0.5 + ofsx;

			F dist = Math::abs(px - gx);

			updateDistance(first, second, dist);

		}

		return applyFlag(first, second) / max * 2 - 1;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 2)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		constexpr F max = 1.41421356237; // sqrt(2)

		point *= frequency;

		F x = point.x;
		F y = point.y;

		i32 ipx = Math::floor(x);
		i32 ipy = Math::floor(y);

		F px = x - ipx;
		F py = y - ipy;

		F first = max;
		F second = max;

		for (const auto& [ofsx, ofsy] : offsets2D) {

			u32 hx = Math::abs(ipx + ofsx) & hashMask;
			u32 hy = Math::abs(ipy + ofsy) & hashMask;

			auto [gx, gy] = gradient<V>[hash(hx, hy) & grad2DMask];

			gx = gx / 2 + 0.5 + ofsx;
			gy = gy / 2 + 0.5 + ofsy;

			F dist = V(px, py).distance(V(gx, gy));

			updateDistance(first, second, dist);

		}

		return applyFlag(first, second) / max * 2 - 1;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 3)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		constexpr F max = 1.73205080756; // sqrt(3)

		point *= frequency;

		F x = point.x;
		F y = point.y;
		F z = point.z;

		i32 ipx = Math::floor(x);
		i32 ipy = Math::floor(y);
		i32 ipz = Math::floor(z);

		F px = x - ipx;
		F py = y - ipy;
		F pz = z - ipz;

		F first = max;
		F second = max;

		for (const auto& [ofsx, ofsy, ofsz] : offsets3D) {

			u32 hx = Math::abs(ipx + ofsx) & hashMask;
			u32 hy = Math::abs(ipy + ofsy) & hashMask;
			u32 hz = Math::abs(ipz + ofsz) & hashMask;

			auto [gx, gy, gz] = gradient<V>[hash(hx, hy, hz) & grad3DMask];

			gx = gx / 2 + 0.5 + ofsx;
			gy = gy / 2 + 0.5 + ofsy;
			gz = gz / 2 + 0.5 + ofsz;

			F dist = V(px, py, pz).distance(V(gx, gy, gz));

			updateDistance(first, second, dist);

		}

		return applyFlag(first, second) / max * 2 - 1;

	}

	template<FloatVector V, Arithmetic A> requires(V::Size == 4)
	constexpr typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;

		constexpr F max = 2; // sqrt(4)

		point *= frequency;

		F x = point.x;
		F y = point.y;
		F z = point.z;
		F w = point.w;

		i32 ipx = Math::floor(x);
		i32 ipy = Math::floor(y);
		i32 ipz = Math::floor(z);
		i32 ipw = Math::floor(w);

		F px = x - ipx;
		F py = y - ipy;
		F pz = z - ipz;
		F pw = w - ipw;

		F first = max;
		F second = max;

		for (const auto& [ofsx, ofsy, ofsz, ofsw] : offsets4D) {

			u32 hx = Math::abs(ipx + ofsx) & hashMask;
			u32 hy = Math::abs(ipy + ofsy) & hashMask;
			u32 hz = Math::abs(ipz + ofsz) & hashMask;
			u32 hw = Math::abs(ipw + ofsw) & hashMask;

			auto [gx, gy, gz, gw] = gradient<V>[hash(hx, hy, hz, hw) & grad3DMask];

			gx = gx / 2 + 0.5 + ofsx;
			gy = gy / 2 + 0.5 + ofsy;
			gz = gz / 2 + 0.5 + ofsz;
			gw = gw / 2 + 0.5 + ofsw;

			F dist = V(px, py, pz, pw).distance(V(gx, gy, gz, gw));

			updateDistance(first, second, dist);

		}

		return applyFlag(first, second) / max * 2 - 1;

	}

	template<class T, Arithmetic A, Arithmetic L, Arithmetic P> requires(Float<T> || FloatVector<T>)
	constexpr auto sample(const T& point, A frequency, u32 octaves, L lacunarity, P persistence) const -> TT::CommonArithmeticType<T> {
		return fractalSample([this](T p, A f) constexpr { return sample(p, f); }, point, frequency, octaves, lacunarity, persistence);
	}

private:

	template<Float F>
	static constexpr void updateDistance(F& first, F& second, F dist) {
		if constexpr (Flag == FlagT::None) {

			first = Math::min(first, dist);

		} else {

			second = Math::min(second, dist);

			if (Math::less(dist, first)) {
				second = first;
				first = dist;
			}

		}
	}


	template<Float F>
	static constexpr F applyFlag(F first, F second) {
		if constexpr (Flag == FlagT::Second) {
			return second;
		} else if constexpr (Flag == FlagT::Diff) {
			return second - first;
		} else {
			return first;
		}
	}


	template<class T, SizeT Size> requires(Integer<T> || IntegerVector<T>)
	static constexpr auto generateOffsets() -> std::array<T, Size> {

		using I = TT::CommonArithmeticType<T>;

		std::array<T, Size> offsets;

		for (u32 i = 0; auto& ofs : offsets) {

			if constexpr (Integer<T>) {

				ofs = T(i % 3 - 1);

			} else {

				constexpr I sizes[] = {1, 3, 9, 27};

				for (u32 j = 0; j < T::Size; j++) {
					ofs[j] = I(i / sizes[j] % 3 - 1);
				}

			}

			i++;

		}

		return offsets;

	}

	static constexpr auto offsets1D = generateOffsets<i32, 3>();

	static constexpr auto offsets2D = generateOffsets<Vec2i, 9>();

	static constexpr auto offsets3D = generateOffsets<Vec3i, 27>();

	static constexpr auto offsets4D = generateOffsets<Vec4i, 81>();

};

using WorleyNoise = WorleyNoiseImpl<WorleyNoiseFlag::None>;
using WorleyNoise2nd = WorleyNoiseImpl<WorleyNoiseFlag::Second>;
using WorleyNoiseDiff = WorleyNoiseImpl<WorleyNoiseFlag::Diff>;
