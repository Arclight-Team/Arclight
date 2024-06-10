/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 NoiseMix.hpp
 */

#pragma once

#include "NoiseBase.hpp"


template<CC::NoiseType... Types>
class NoiseMix {

	template<class T, class... Pack>
	using ArgsHelper = T;

public:

	static constexpr u32 TypesCount = sizeof...(Types);

	static_assert(TypesCount > 1, "Cannot mix less than 2 noise types");

	template<CC::Arithmetic C, SizeT N> requires(N == TypesCount - 1)
	using ContributionT = const C (&)[N];


	constexpr NoiseMix(const Types&... types) : types(types...) {};

	constexpr NoiseMix() = default;


	template<SizeT I> requires(I < TypesCount)
	inline void permutate(u32 seed) {
		std::get<I>(types).permutate(seed);
	}

	template<SizeT I> requires(I < TypesCount)
	inline void permutate() {
		std::get<I>(types).permutate();
	}


	template<SizeT I> requires(I < TypesCount)
	constexpr TT::NthPackType<I, Types...> get() {
		return std::get<I>(types);
	}


	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L = u32, CC::Arithmetic P = u32, CC::Float F = TT::ExtractType<T>>
	constexpr F sample(const T& point, A frequency, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {

		F sample;

		std::apply([&](const auto&... args) constexpr {
			sample = (args.sample(point, frequency, octaves, lacunarity, persistence) + ...);
		}, types);

		return sample / TypesCount;

	}

	template<CC::Arithmetic C, SizeT N, CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L = u32, CC::Arithmetic P = u32, CC::Float F = TT::ExtractType<T>>
	constexpr F sample(ContributionT<C, N> contribution, const T& point, A frequency, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {

		F sample = 0;

		auto calculate = [&](const auto& type, u32 idx) constexpr {

			F c = (idx == 0) ? 1 : contribution[idx - 1];

			sample += type.sample(point, frequency, octaves, lacunarity, persistence) * c;

			if (idx != TypesCount - 1) {
				sample *= (1 - contribution[idx]);
			}

		};

		std::apply([&](const auto&... args) constexpr {
			u32 idx = 0;
			(calculate(args, idx++), ...);
		}, types);

		return sample;

	}

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L = u32, CC::Arithmetic P = u32, CC::Float F = TT::ExtractType<T>, CC::Returns<F, ArgsHelper<F, Types>...> Func>
	constexpr F sample(Func&& transform, const T& point, A frequency, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {

		F sample;

		std::apply([&](const auto&... args) constexpr {
			sample = transform(args.sample(point, frequency, octaves, lacunarity, persistence)...);
		}, types);

		return sample;

	}


	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L = u32, CC::Arithmetic P = u32, CC::Float F = TT::ExtractType<T>>
	constexpr std::vector<F> sample(std::span<const T> points, std::span<const A> frequencies, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {

		arc_assert(points.size() == frequencies.size(), "The amount of points need to match the amount of frequencies");

		const u32 count = points.size();

		std::vector<F> out(count);

		auto calculate = [&](const auto& type) constexpr {

			std::vector<F> samples = type.sample(points, frequencies, octaves, lacunarity, persistence);

			for (u32 i = 0; i < count; i++) {
				out[i] += samples[i];
			}

		};

		std::apply([&](const auto&... args) constexpr {
			(calculate(args), ...);
		}, types);

		for (F& sample : out) {
			sample /= TypesCount;
		}

		return out;

	}

	template<CC::Arithmetic C, SizeT N, CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L = u32, CC::Arithmetic P = u32, CC::Float F = TT::ExtractType<T>>
	constexpr std::vector<F> sample(ContributionT<C, N> contribution, std::span<const T> points, std::span<const A> frequencies, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {

		arc_assert(points.size() == frequencies.size(), "The amount of points need to match the amount of frequencies");

		const u32 count = points.size();

		std::vector<F> out(count);

		auto calculate = [&](const auto& type, u32 idx) constexpr {

			F scale = (idx == 0) ? 1 : contribution[idx - 1];

			std::vector<F> samples = type.sample(points, frequencies, octaves, lacunarity, persistence);

			for (u32 i = 0; i < count; i++) {

				out[i] += samples[i] * scale;

				if (idx != TypesCount - 1) {
					out[i] *= 1 - contribution[idx];
				}
			}

		};

		std::apply([&](const auto&... args) constexpr {
			u32 idx = 0;
			(calculate(args, idx++), ...);
		}, types);

		return out;

	}

	template<CC::FloatParam T, CC::Arithmetic A, CC::Arithmetic L = u32, CC::Arithmetic P = u32, CC::Float F = TT::ExtractType<T>, CC::Returns<F, ArgsHelper<F, Types>...> Func>
	constexpr std::vector<F> sample(Func&& transform, std::span<const T> points, std::span<const A> frequencies, u32 octaves = 1, L lacunarity = 1, P persistence = 1) const {

		arc_assert(points.size() == frequencies.size(), "The amount of points need to match the amount of frequencies");

		const u32 count = points.size();

		std::vector<F> out(count);

		auto calculate = [&](const auto&... samples) constexpr {

			for (u32 i = 0; i < count; i++) {
				out[i] = transform(samples[i]...);
			}

		};

		std::apply([&](const auto&... args) constexpr {
			calculate(args.sample(points, frequencies, octaves, lacunarity, persistence)...);
		}, types);

		return out;

	}

private:

	std::tuple<Types...> types;

};
