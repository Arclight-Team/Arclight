/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 simplex.hpp
 */

#pragma once

#include "noisebase.hpp"


class SimplexNoise : public NoiseBase<SimplexNoise> {

public:

	using NoiseBase<SimplexNoise>::sample;


	template<CC::Float F, CC::Arithmetic A>
	F sample(F point, A frequency) const {

		point *= frequency;

		i32 ip = Math::floor(point);

		F p = point - ip;

		ip &= hashMask;

		auto part = [&](const u32& offset) {

			i32 ofsP = ip + offset;

			F dot = grad1D[hash(ofsP) & grad1DMask] * (p - offset);

			return dot * falloff(p - offset, 1);

		};

		F sample = part(0) + part(1);

		constexpr float scale = 64.0 / 27;

		return sample * scale;

	}

	template<FloatVector V, CC::Arithmetic A> requires(V::Size == 2)
	typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;
		using Vi = Vec2i;
		using Vui = Vec2ui;

		constexpr F toTriangle = 0.2113248654;
		constexpr F toSquare = 0.36602540378;

		point *= frequency;

		F skew = (point.x + point.y) * toSquare;
		V skewed = point + V(skew);

		Vi ip = Math::floor(skewed);
		Vui hashP = ip & hashMask;

		auto part = [&](const Vui& offset) {

			Vi ofsP = ip + offset;

			F unskew = (ofsP.x + ofsP.y) * toTriangle;
			V unskewed = point - ofsP + V(unskew);

			F dot = grad2D[hash(hashP + offset) & grad2DMask].dot(unskewed);

			return dot * falloff(unskewed, 0.5);

		};

		F sample = part({0, 0}) + part({1, 1});

		V diff = skewed - ip;

		if (diff.x >= diff.y) {
			sample += part({1, 0});
		} else {
			sample += part({0, 1});
		}

		constexpr float scale = 32.990773983; // 2916 * sqrt(2) / 125

		return sample * scale;

	}

	template<FloatVector V, CC::Arithmetic A> requires(V::Size == 3)
	typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;
		using Vi = Vec3i;
		using Vui = Vec3ui;

		constexpr F toTetrahedron = 1.0 / 6;
		constexpr F toCube = 1.0 / 3;

		point *= frequency;

		F skew = (point.x + point.y + point.z) * toCube;
		V skewed = point + V(skew);

		Vi ip = Math::floor(skewed);
		Vui hashP = ip & hashMask;

		auto part = [&](const Vui& offset) {

			Vi ofsP = ip + offset;

			F unskew = (ofsP.x + ofsP.y + ofsP.z) * toTetrahedron;
			V unskewed = point - ofsP + V(unskew);

			F dot = grad3D[hash(hashP + offset) & grad3DMask].dot(unskewed);

			return dot * falloff(unskewed, 0.5);

		};

		V diff = skewed - ip;

		bool xy = diff.x >= diff.y;
		bool xz = diff.x >= diff.z;
		bool yz = diff.y >= diff.z;

		F sample12 = part({0, 0, 0}) + part({1, 1, 1});
		F sample3;
		F sample4;

		if (!xy && yz) {
			sample3 = part({0, 1, 0});

			if (xz) {
				sample4 = part({1, 1, 0});
			} else {
				sample4 = part({0, 1, 1});
			}
		} else if (xz) {
			sample3 = part({1, 0, 0});

			if (yz) {
				sample4 = part({1, 1, 0});
			} else {
				sample4 = part({1, 0, 1});
			}
		} else {
			sample3 = part({0, 0, 1});

			if (xy) {
				sample4 = part({1, 0, 1});
			} else {
				sample4 = part({0, 1, 1});
			}
		}

		constexpr float scale = 30.6822935365; // 8192 * sqrt(3) / (327 * sqrt(2))

		return (sample12 + sample3 + sample4) * scale;

	}

	template<FloatVector V, CC::Arithmetic A> requires(V::Size == 4)
	typename V::Type sample(V point, A frequency) const {

		using F = typename V::Type;
		using Vi = Vec4i;
		using Vui = Vec4ui;

		constexpr F to5Cell = 0.13819660112;
		constexpr F toTesseract = 0.30901699437;

		point *= frequency;

		F skew = (point.x + point.y + point.z + point.w) * toTesseract;
		V skewed = point + V(skew);

		Vi ip = Math::floor(skewed);
		Vui hashP = ip & hashMask;

		auto part = [&](const Vui& offset) {

			Vi ofsP = ip + offset;

			F unskew = (ofsP.x + ofsP.y + ofsP.z + ofsP.w) * to5Cell;
			V unskewed = point - ofsP + V(unskew);

			F dot = grad4D[hash(hashP + offset) & grad4DMask].dot(unskewed);

			return dot * falloff(unskewed, 0.5);

		};

		V diff = skewed - ip;

		bool xy = diff.x >= diff.y;
		bool xz = diff.x >= diff.z;
		bool yz = diff.y >= diff.z;
		bool xw = diff.x >= diff.w;
		bool yw = diff.y >= diff.w;
		bool zw = diff.z >= diff.w;

		F sample12 = part({0, 0, 0, 0}) + part({1, 1, 1, 1});
		F sample3;
		F sample4;
		F sample5;

		if (!xy && yw && yz) {
			sample3 = part({0, 1, 0, 0});

			if (!xw && !zw) {
				sample4 = part({0, 1, 0, 1});

				if (xz) {
					sample5 = part({1, 1, 0, 1});
				} else {
					sample5 = part({0, 1, 1, 1});
				}
			} else if (xz && xw) {
				sample4 = part({1, 1, 0, 0});

				if (!zw) {
					sample5 = part({1, 1, 0, 1});
				} else {
					sample5 = part({1, 1, 1, 0});
				}
			} else {
				sample4 = part({0, 1, 1, 0});

				if (!xw) {
					sample5 = part({0, 1, 1, 1});
				} else {
					sample5 = part({1, 1, 1, 0});
				}
			}
		} else if (!xw && !yw && !zw) {
			sample3 = part({0, 0, 0, 1});

			if (!xy && yz) {
				sample4 = part({0, 1, 0, 1});

				if (!xz) {
					sample5 = part({0, 1, 1, 1});
				} else {
					sample5 = part({1, 1, 0, 1});
				}
			} else if (xz && xy) {
				sample4 = part({1, 0, 0, 1});

				if (yz) {
					sample5 = part({1, 1, 0, 1});
				} else {
					sample5 = part({1, 0, 1, 1});
				}
			} else {
				sample4 = part({0, 0, 1, 1});

				if (!xy) {
					sample5 = part({0, 1, 1, 1});
				} else {
					sample5 = part({1, 0, 1, 1});
				}
			}
		} else if (!yz && !xz && zw) {
			sample3 = part({0, 0, 1, 0});

			if (!xy && yw) {
				sample4 = part({0, 1, 1, 0});

				if (!xw) {
					sample5 = part({0, 1, 1, 1});
				} else {
					sample5 = part({1, 1, 1, 0});
				}
			} else if (!xw && !yw) {
				sample4 = part({0, 0, 1, 1});

				if (!xy) {
					sample5 = part({0, 1, 1, 1});
				} else {
					sample5 = part({1, 0, 1, 1});
				}
			} else {
				sample4 = part({1, 0, 1, 0});

				if (yw) {
					sample5 = part({1, 1, 1, 0});
				} else {
					sample5 = part({1, 0, 1, 1});
				}
			}
		} else {
			sample3 = part({1, 0, 0, 0});

			if (yz && yw) {
				sample4 = part({1, 1, 0, 0});

				if (!zw) {
					sample5 = part({1, 1, 0, 1});
				} else {
					sample5 = part({1, 1, 1, 0});
				}
			} else if (!yz && zw) {
				sample4 = part({1, 0, 1, 0});

				if (yw) {
					sample5 = part({1, 1, 1, 0});
				} else {
					sample5 = part({1, 0, 1, 1});
				}
			} else {
				sample4 = part({1, 0, 0, 1});

				if (yz) {
					sample5 = part({1, 1, 0, 1});
				} else {
					sample5 = part({1, 0, 1, 1});
				}
			}
		}

		constexpr float scale = 27;

		return (sample12 + sample3 + sample4 + sample5) * scale;

	}

private:

	template<class T, CC::Arithmetic B> requires(CC::Float<T> || FloatVector<T>)
	static constexpr auto falloff(const T& v, B bias) -> TT::CommonArithmeticType<T> {

		using F = TT::CommonArithmeticType<T>;

		F a = F(bias);

		if constexpr (CC::Float<T>) {

			a -= v * v;

		} else {

			for (u32 i = 0; i < T::Size; i++) {
				a -= v[i] * v[i];
			}

		}

		if (a > 0) {
			return a * a * a;
		} else {
			return F(0);
		}
	}

};
