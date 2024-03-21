/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Collate.hpp
 */

#pragma once

#include "Unicode.hpp"

#include <compare>


/*
 *  Unicode Collation is a vast topic, therefore we only implement codepoint collation for now (i.e. sorting by codepoint index).
 *  This completely ignores compound codepoints, locale-specific collation and ordering of different scripts.
 */
namespace Unicode {

	constexpr std::strong_ordering collateByCodepoint(Codepoint cpLeft, Codepoint cpRight) {
		return cpLeft <=> cpRight;
	}

	enum class CollationMode {
		Codepoint
	};
/*
	template<CollationMode Mode, Encoding E0, Encoding E1, class T0 = typename UTFEncodingTraits<E0>::Type, class T1 = typename UTFEncodingTraits<E0>::Type>
	std::strong_ordering collate(const T0* s0, const T0* e0, const T1* s1, const T1* e1) {

	}
*/
}