/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 worley.hpp
 */

#pragma once

#include "noisebase.hpp"


class WorleyNoise : public NoiseBase<WorleyNoise> {

public:

	using NoiseBase<WorleyNoise>::sample;


	template<bool Negative = false, Float F, Arithmetic A>
	F sample(F point, A frequency) const {

		if constexpr (!Negative) {
			arc_assert(point < Zero, "Attempted to generate worley noise from unsupported negative point")
		}

		point *= frequency;

		i32 ip = Math::floor(point);

		F p = point - ip;

		constexpr F offsets[] = {-1, 0, 1};

		F sample = 2;

		for (u32 i = 0; i < 3; i++) {

			u32 h = Math::abs(ip + offsets1D[i]);
			h &= hashMask;

			F grad = grad1D[hash(h) & grad1DMask] / 2;
			grad += offsets1D[i] + F(0.5);

			sample = Math::min(sample, Math::abs(p - grad));

		}

		return sample - 1;

	}

	template<bool Negative = false, FloatVector V, Arithmetic A> requires(V::Size == 2)
	typename V::Type sample(V point, A frequency) const {

		if constexpr (!Negative) {
			arc_assert(!point.anyNegative(), "Attempted to generate worley noise from unsupported negative point")
		}

		using F = typename V::Type;
		using Vi = Vec2i;
		using Vui = Vec2ui;

		point *= frequency;

		Vi ip = Math::floor(point);

		V p = point - ip;

		F sample = 2.82842712475; // 2 * sqrt(2)

		for (u32 i = 0; i < 3 * 3; i++) {

			Vui h = Math::abs(ip + offsets2D[i]);
			h &= hashMask;

			V grad = grad2D[hash(h) & grad2DMask] / 2;
			grad += offsets2D[i] + V(0.5);

			sample = Math::min(sample, p.distance(grad));

		}

		return sample - 1;

	}

	template<bool Negative = false, FloatVector V, Arithmetic A> requires(V::Size == 3)
	typename V::Type sample(V point, A frequency) const {

		if constexpr (!Negative) {
			arc_assert(!point.anyNegative(), "Attempted to generate worley noise from unsupported negative point")
		}

		using F = typename V::Type;
		using Vi = Vec3i;
		using Vui = Vec3ui;

		point *= frequency;

		Vi ip = Math::floor(point);

		V p = point - ip;

		F sample = 3.46410161514; // 2 * sqrt(3)

		for (u32 i = 0; i < 3 * 3 * 3; i++) {

			Vui h = Math::abs(ip + offsets3D[i]);
			h &= hashMask;

			V grad = grad3D[hash(h) & grad3DMask] / 2;
			grad += offsets3D[i] + V(0.5);

			sample = Math::min(sample, p.distance(grad));

		}

		return sample - 1;

	}

	template<bool Negative = false, FloatVector V, Arithmetic A> requires(V::Size == 4)
	typename V::Type sample(V point, A frequency) const {

		if constexpr (!Negative) {
			arc_assert(!point.anyNegative(), "Attempted to generate worley noise from unsupported negative point")
		}

		using F = typename V::Type;
		using Vi = Vec4i;
		using Vui = Vec4ui;

		point *= frequency;

		Vi ip = Math::floor(point);

		V p = point - ip;

		F sample = 4;

		for (u32 i = 0; i < 3 * 3 * 3 * 3; i++) {

			Vui h = Math::abs(ip + offsets4D[i]);
			h &= hashMask;

			V grad = grad4D[hash(h) & grad4DMask] / 2;
			grad += offsets4D[i] + V(0.5);

			sample = Math::min(sample, p.distance(grad));

		}

		return sample - 1;

	}

private:

	template<class T, u32 Size> requires(Integer<T> || IntegerVector<T>)
	static constexpr auto generateOffsets() -> std::array<T, Size> {

		using I = TT::CommonArithmeticType<T>;

		std::array<T, Size> offsets;

		for (u32 i = 0; i < Size; i++) {
			if constexpr (Integer<T>) {

				offsets[i] = T(i % 3 - 1);

			} else {

				constexpr I sizes[] = {1, 3, 9, 27};

				for (u32 j = 0; j < T::Size; j++) {
					offsets[i][j] = I(i / sizes[j] % 3 - 1);
				}

			}
		}

		return offsets;

	}

	static constexpr auto offsets1D = generateOffsets<i32, 3>();

	static constexpr auto offsets2D = generateOffsets<Vec2i, 9>();

	static constexpr auto offsets3D = generateOffsets<Vec3i, 27>();

	static constexpr auto offsets4D = generateOffsets<Vec4i, 81>();

};
