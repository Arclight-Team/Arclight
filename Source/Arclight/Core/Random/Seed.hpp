/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Seed.hpp
 */

#pragma once

#include "EntropySource.hpp"
#include "Meta/TypeTraits.hpp"
#include "Meta/Concepts.hpp"
#include "Math/Math.hpp"
#include "StdExt/BitSpan.hpp"
#include "Util/Bits.hpp"
#include "Common/Types.hpp"

#include <span>
#include <vector>



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

		SizeT bytes = (Bits + 7) / 8;

		if constexpr (ExtStorage) {
			storage.resize(bytes);
		} else {
			storage = 0;
		}

	}

	template<CC::Integer I>
	constexpr Seed(I i) {

		SizeT bytes = (Bits + 7) / 8;

		if constexpr (ExtStorage) {

			storage.resize(bytes);
			Bits::disassemble(i, storage.data(), storage.size());

		} else {
			storage = i;
		}

	}

	template<CC::Integer I>
	constexpr Seed(std::initializer_list<const I> i) : Seed(std::span{i}) {}

	template<CC::Integer I>
	constexpr Seed(std::span<const I> i) {

		if constexpr (ExtStorage) {

			SizeT destBytes = (Bits + 7) / 8;
			storage.resize(destBytes);

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

	static Seed fromEntropySource() {

		Seed seed;
		EntropySource source;
		SizeT bytes = (Bits + 7) / 8;

		if constexpr (ExtStorage) {
			source.fetch(seed.storage);
		} else {
			source.fetch({ Bits::toByteArray(&seed.storage), bytes });
		}

		return seed;

	}

};


using Seed32 = Seed<32>;
using Seed64 = Seed<64>;