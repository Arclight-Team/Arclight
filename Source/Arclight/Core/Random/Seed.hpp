/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Seed.hpp
 */

#pragma once

#include "EntropySource.hpp"
#include "Common/Types.hpp"
#include "Math/Math.hpp"
#include "Meta/Concepts.hpp"
#include "StdExt/BitSpan.hpp"
#include "Util/Bits.hpp"

#include <span>
#include <vector>
#include <charconv>



namespace SeedDetail {

	//Unsigned seed storage type
	using SeedT = u64;

	template<SizeT Bits>
	struct SeedStorageHelper {
		constexpr static bool Large = Bits > Bits::bitCount<SeedT>();
	};

	template<bool Large>
	struct SeedStorage {};

	template<> struct SeedStorage<false>    { SeedT storage; };
	template<> struct SeedStorage<true>     { std::vector<u8> storage; };

}



template<SizeT Bits> requires (Bits > 0)
class Seed : private SeedDetail::SeedStorage<SeedDetail::SeedStorageHelper<Bits>::Large> {

	using Base = SeedDetail::SeedStorage<SeedDetail::SeedStorageHelper<Bits>::Large>;
	using Base::storage;

	constexpr static bool ExtStorage = SeedDetail::SeedStorageHelper<Bits>::Large;

public:

	constexpr Seed() {

		if constexpr (ExtStorage) {
			storage.resize(sizeBytes());
		} else {
			storage = 0;
		}

	}

	template<CC::Integer I>
	constexpr explicit Seed(I i) {

		if constexpr (ExtStorage) {

			storage.resize(sizeBytes());
			Bits::disassemble(i, storage.data(), storage.size());

		} else {
			storage = i;
		}

	}

	template<CC::Integer I>
	constexpr explicit Seed(std::initializer_list<const I> i) : Seed(std::span{i}) {}

	template<CC::Integer I>
	constexpr explicit Seed(std::span<const I> i) {

		if constexpr (ExtStorage) {

			storage.resize(sizeBytes());

			for (SizeT n = 0; n < i.size(); n++) {

				SizeT offset = sizeof(I) * n;

				if (Bits::disassemble(i[n], storage.data() + offset, storage.size() - offset) != sizeof(I)) {
					break;
				}

			}

		} else {

			using SeedT = SeedDetail::SeedT;

			if constexpr (sizeof(I) >= sizeof(SeedT)) {

				storage = i.empty() ? 0 : i[0];

			} else {

				constexpr SizeT inBits = Bits::bitCount<I>();
				storage = Bits::assemble<SeedT>(i.data(), Math::min(i.size(), (Bits + inBits - 1) / inBits));

			}

		}

	}

	template<CC::Integral T>
	constexpr T get(SizeT offset) const noexcept {

		if constexpr (ExtStorage) {
			return BitSpan(storage.data(), offset, Bits - offset).template read<T>();
		} else {
			return BitSpan(&storage, offset, Bits - offset).template read<T>();
		}

	}

	constexpr std::span<const u8> span() const noexcept {

		if constexpr (ExtStorage) {
			return { storage.data(), sizeBytes() };
		} else {
			return { Bits::toByteArray(&storage), sizeBytes() };
		}

	}

	constexpr bool fromString(const std::string& s) noexcept {

		if constexpr (ExtStorage) {

			if (s.size() < storage.size() * 2) {
				return false;
			}

			for (SizeT i = 0; u8& b : storage) {

				if (std::from_chars(s.data() + i, s.data() + i + 2, b, 16).ec != std::errc {}) {
					return false;
				}

				i += 2;

			}

			return true;

		} else {

			return std::from_chars(s.begin(), s.end(), storage, 16).ec == std::errc {};

		}

	}

	constexpr std::string toString() const noexcept {

		if constexpr (ExtStorage) {

			std::string s;

			for (u8 b : storage) {
				s += String::toHexString(b, String::HexFlags::Fill);
			}

			return s;

		} else {

			return String::toHexString(Bits::big(storage), String::HexFlags::Fill);

		}

	}

	static SizeT sizeBytes() {
		return (Bits + 7) / 8;
	}

	static Seed fromEntropySource() {

		Seed seed;
		EntropySource source;

		if constexpr (ExtStorage) {
			source.fetch(seed.storage);
		} else {
			source.fetch({ Bits::toByteArray(&seed.storage), sizeBytes() });
		}

		return seed;

	}

};


using Seed32 = Seed<32>;
using Seed64 = Seed<64>;