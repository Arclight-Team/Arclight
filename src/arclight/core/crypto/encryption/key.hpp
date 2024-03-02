/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 key.hpp
 */

#pragma once

#include "math/math.hpp"
#include "util/bits.hpp"
#include "util/string.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
#include "common/types.hpp"

#include <array>
#include <string>



template<SizeT Size>
class Key {

	constexpr static SizeT Segments = (Size + 7) / 8;

public:

	constexpr Key() noexcept {

		for(SizeT i = 0; i < Segments; i++) {
			segments[i] = 0;
		}

	}

	template<CC::Integer... J>
	constexpr Key(J... j) noexcept {
		set(j...);
	}

	template<CC::Integer... J> requires ((sizeof(J) + ...) * 8 == Size)
	constexpr void set(J... j) {

		SizeT i = 0;
		((Bits::disassemble(j, segments + i), i += sizeof(J)), ...);

	}

	constexpr bool operator==(const Key<Size>& other) noexcept {

		bool equal = true;

		for(SizeT i = 0; i < Segments; i++) {
			equal &= segments[i] == other.segments[i];
		}

		return equal;

	}

	template<CC::UnsignedType U>
	constexpr U toInteger(SizeT start = 0) const noexcept {

		if (start < Segments) {
			return Bits::assemble<U>(segments, Segments - start);
		} else {
			return 0;
		}

	}

	constexpr std::array<u8, Size / 8> toArray() const noexcept {

		std::array<u8, Size / 8> a;

		for(SizeT i = 0; i < Segments; i++) {
			Bits::disassemble(segments[i], a.data() + i);
		}

		return a;

	}

	constexpr BitSpan<Size> bits() const noexcept {
		return segments;
	}

	constexpr BitSpan<Size, false> bits() noexcept {
		return segments;
	}

	constexpr auto operator[](SizeT bit) const noexcept {
		return bits()[bit];
	}

	constexpr auto operator[](SizeT bit) noexcept {
		return bits()[bit];
	}

	template<SizeT Offset, SizeT Count = Size> requires((Offset + Count) <= Size)
	constexpr auto subKey() const noexcept {

		Key<Count> key;

		auto myBits = bits();
		auto outBits = key.bits();


		for (u32 i = 0; i < Count; i++) {
			outBits[i] = myBits[i + Offset];
		}
		
		//key[i] = (*this)[i + Offset];

		return key;

	}

	std::string toString(bool upper = false) const noexcept {

		std::string s;

		for(SizeT i = 0; i < Segments; i++) {
			s += String::toHexString(Bits::swap(segments[i]), upper, false);
		}

		return s;

	}

private:

	u8 segments[Segments];

};