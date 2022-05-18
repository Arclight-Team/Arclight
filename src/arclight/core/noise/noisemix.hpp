/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 noisemix.hpp
 */


#include "noisebase.hpp"


template<CC::NoiseType... Types>
class NoiseMix {

public:

	static constexpr u32 TypesCount = sizeof...(Types);

	static_assert(TypesCount > 1, "Cannot mix less than 2 noise types");

	template<CC::Arithmetic C, SizeT N> requires(N == TypesCount - 1)
	using ContributionT = const C (&)[N];


	NoiseMix(const Types&... types) : types(types...) {};

	NoiseMix() = default;


	template<SizeT I> requires(I < TypesCount)
	inline void permutate(u32 seed) {
		std::get<I>(types).permutate(seed);
	}

	template<SizeT I> requires(I < TypesCount)
	inline void permutate() {
		std::get<I>(types).permutate();
	}


	template<class T, CC::Arithmetic A> requires(CC::Float<T> || CC::FloatVector<T>)
	constexpr auto sample(const T& point, A frequency) const -> TT::CommonArithmeticType<T> {

		using F = TT::CommonArithmeticType<T>;

		F sample = 0;

		std::apply([&](const auto&... args) constexpr {
			sample = (args.sample(point, frequency) + ...);
		}, types);

		return sample / TypesCount;

	}

	template<CC::Arithmetic C, SizeT N, class T, CC::Arithmetic A> requires(CC::Float<T> || CC::FloatVector<T>)
	constexpr auto sample(ContributionT<C, N> contribution, const T& point, A frequency) const -> TT::CommonArithmeticType<T> {

		using F = TT::CommonArithmeticType<T>;

		F sample = 0;

		auto calculate = [&](const auto& type, u32 i) constexpr {

			F c = (i == 0) ? 1 : contribution[i - 1];

			sample += type.sample(point, frequency) * c;

			if (i != TypesCount - 1) {
				sample *= (1 - contribution[i]);
			}

		};

		std::apply([&](const auto&... args) constexpr {
			u32 i = 0;
			(calculate(args, i++), ...);
		}, types);

		return sample;

	}

	template<class T, CC::Arithmetic A, class Func, class... Args> requires((CC::Float<T> || CC::FloatVector<T>) && CC::Invocable<Func, Args...> && TT::IsAllSame<T, Args...>)
	constexpr auto sample(const T& point, A frequency, Func transform) const -> TT::CommonArithmeticType<T> {

		using F = TT::CommonArithmeticType<T>;

		F sample = 0;

		std::apply([&](const auto&... args) constexpr {
			sample = transform(args.sample(point, frequency)...);
		}, types);

		return sample;

	}


	template<class T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P> requires(CC::Float<T> || CC::FloatVector<T>)
	constexpr auto sample(const T& point, A frequency, u32 octaves, L lacunarity, P persistence) const -> TT::CommonArithmeticType<T> {

		using F = TT::CommonArithmeticType<T>;

		F sample = 0;

		std::apply([&](const auto&... args) constexpr {
			sample = (args.sample(point, frequency, octaves, lacunarity, persistence) + ...);
		}, types);

		return sample / TypesCount;

	}

	template<CC::Arithmetic C, SizeT N, class T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P> requires(CC::Float<T> || CC::FloatVector<T>)
	constexpr auto sample(ContributionT<C, N> contribution, const T& point, A frequency, u32 octaves, L lacunarity, P persistence) const -> TT::CommonArithmeticType<T> {

		using F = TT::CommonArithmeticType<T>;

		F sample = 0;

		auto calculate = [&](const auto& type, u32 i) constexpr {

			F c = (i == 0) ? 1 : contribution[i - 1];

			sample += type.sample(point, frequency, octaves, lacunarity, persistence) * c;

			if (i != TypesCount - 1) {
				sample *= (1 - contribution[i]);
			}

		};

		std::apply([&](const auto&... args) constexpr {
			u32 i = 0;
			(calculate(args, i++), ...);
		}, types);

		return sample;

	}

	template<class T, CC::Arithmetic A, CC::Arithmetic L, CC::Arithmetic P, class Func, class... Args> requires((CC::Float<T> || CC::FloatVector<T>) && CC::Invocable<Func, Args...> && TT::IsAllSame<T, Args...>)
	constexpr auto sample(const T& point, A frequency, u32 octaves, L lacunarity, P persistence, Func transform) const -> TT::CommonArithmeticType<T> {

		using F = TT::CommonArithmeticType<T>;

		F sample = 0;

		std::apply([&](const auto&... args) constexpr {
			sample = transform(args.sample(point, frequency, octaves, lacunarity, persistence)...);
		}, types);

		return sample / TypesCount;

	}

private:

	std::tuple<Types...> types;
};
