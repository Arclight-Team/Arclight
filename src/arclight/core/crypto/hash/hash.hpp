/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 hash.hpp
 */

#pragma once

#include "math/math.hpp"
#include "util/bits.hpp"
#include "util/string.hpp"
#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "types.hpp"

#include <array>
#include <string>



template<SizeT Size>
class Hash {

	constexpr static SizeT Segments = Size / 8;

public:

	constexpr Hash() noexcept {

		for(SizeT i = 0; i < Segments; i++) {
			segments[i] = 0;
		}

	}

	template<Integer... J>
	constexpr Hash(J... j) noexcept {
		set(j...);
	}

	template<Integer... J> requires ((sizeof(J) + ...) * 8 == Size)
	constexpr void set(J... j) {

		SizeT i = 0;
		((Bits::disassemble(j, segments + i), i += sizeof(J)), ...);

	}

	constexpr bool operator==(const Hash<Size>& other) noexcept {

		bool equal = true;

		for(SizeT i = 0; i < Segments; i++) {
			equal &= segments[i] == other.segments[i];
		}

		return equal;

	}

	template<UnsignedType U>
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