/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 PixelTraits.hpp
 */

#pragma once

#include "Channel.hpp"
#include "Math/Saturate.hpp"
#include "Math/Math.hpp"



template<Channel::Type>
struct PixelTraits {};



template<>
struct PixelTraits<Channel::Type::Unsigned> {

	template<u32 B>
	using PixelDataType = TT::UnsignedFromMinSize<Bits::ceilPowerOf2(B, 8) / 8>;

	template<u32 B>
	constexpr static PixelDataType<B> Zero = 0;

	template<u32 B>
	constexpr static PixelDataType<B> One = Bits::ones<PixelDataType<B>>(B);



	template<u32 B, class PixelType = PixelDataType<B>>
	constexpr static PixelType add(PixelType p, PixelType q) noexcept {

		constexpr bool Saturate = B > 4 && Bits::isPowerOf2(B);
		PixelType op = 0;

		if constexpr (Saturate) {

			op = Math::saturateAdd<PixelType>(p, q);

		} else {

			op = p + q;

			if (op > Bits::ones<PixelType>(B)) {
				op = Bits::ones<PixelType>(B);
			}

		}

		return op;

	}

	template<u32 B, class PixelType = PixelDataType<B>>
	constexpr static PixelType subtract(PixelType p, PixelType q) noexcept {
		return p >= q ? p - q : 0;
	}

	template<u32 B, CC::Arithmetic S, class PixelType = PixelDataType<B>>
	constexpr static PixelType multiply(PixelType p, S s) noexcept {

		constexpr bool Saturated = B > 4 && Bits::isPowerOf2(B);
		PixelType op = 0;

		if constexpr (CC::UnsignedType<S>) {

			op = Math::saturateMultiply<PixelType>(p, s);

			if (!Saturated) {

				if (op >= Bits::ones<PixelType>(B)) {
					op = Bits::ones<PixelType>(B);
				}

			}

		} else if constexpr (CC::SignedType<S>) {

			if (s < 0) {

				op = 0;

			} else {

				op = Math::saturateMultiply<PixelType>(p, s);

				if (!Saturated) {

					if (op >= Bits::ones<PixelType>(B)) {
						op = Bits::ones<PixelType>(B);
					}

				}

			}

		} else if constexpr (CC::Float<S>) {

			using OpType = TT::CommonType<PixelType, S>;

			OpType t = p * s;

			if (t < 0) {
				op = 0;
			} else if (t > Bits::ones<PixelType>(B)) {
				op = Bits::ones<PixelType>(B);
			} else {
				op = Math::round(t);
			}

		}

		return op;

	}

	template<u32 B, CC::Arithmetic S, class PixelType = PixelDataType<B>>
	constexpr static PixelType divide(PixelType p, S s) noexcept {

		if constexpr (CC::UnsignedType<S>) {

			return p / s;

		} else if constexpr (CC::SignedType<S>) {

			return s >= 0 ? p / s : 0;

		} else if constexpr (CC::Float<S>) {

			using OpType = TT::CommonType<PixelType, S>;

			OpType t = p / s;

			if (t < 0) {
				return 0;
			}

			if (t > Bits::ones<PixelType>(B)) {
				return Bits::ones<PixelType>(B);
			}

			return Math::round(t);

		}

		std::unreachable();

	}

};



template<>
struct PixelTraits<Channel::Type::Float> {

	// Only float32 and float64 so far, let compilation fail if B is anything else
	template<u32 B>
	using PixelDataType = TT::Conditional<B == Bits::bitCount<float>(), float, TT::Conditional<B == Bits::bitCount<double>(), double, void>>;

	template<u32 B>
	constexpr static PixelDataType<B> Zero = 0;

	template<u32 B>
	constexpr static PixelDataType<B> One = 1;



	template<u32 B, class PixelType = PixelDataType<B>>
	constexpr static PixelType add(PixelType p, PixelType q) noexcept {
		return p + q;
	}

	template<u32 B, class PixelType = PixelDataType<B>>
	constexpr static PixelType subtract(PixelType p, PixelType q) noexcept {
		return p - q;
	}

	template<u32 B, CC::Arithmetic S, class PixelType = PixelDataType<B>>
	constexpr static PixelType multiply(PixelType p, S s) noexcept {
		return p * s;
	}

	template<u32 B, CC::Arithmetic S, class PixelType = PixelDataType<B>>
	constexpr static PixelType divide(PixelType p, S s) noexcept {
		return p / s;
	}

};