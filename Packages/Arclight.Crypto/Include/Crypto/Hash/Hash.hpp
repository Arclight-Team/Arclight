/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Hash.hpp
 */

#pragma once

#include "Math/Math.hpp"
#include "StdExt/BitSpan.hpp"
#include "Util/Bits.hpp"
#include "Util/String.hpp"
#include "Meta/Concepts.hpp"
#include "Meta/TypeTraits.hpp"
#include "Common/Types.hpp"

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

	template<CC::Integer... J>
	constexpr Hash(J... j) noexcept {
		set(j...);
	}

	template<CC::Integer... J> requires ((sizeof(J) + ...) * 8 == Size)
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

	template<CC::UnsignedType U>
	constexpr U toInteger(SizeT start = 0) const noexcept {

		if (start < Segments) {
			return Bits::assemble<U>(segments + start, Segments - start);
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