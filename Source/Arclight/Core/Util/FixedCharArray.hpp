/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 FixedCharArray.hpp
 */

#pragma once

#include "Meta/Concepts.hpp"
#include "Common/Types.hpp"



template<CC::Char C, SizeT N>
class FixedCharArray {

public:

    using CharT = C;

	constexpr static SizeT Size = N;

    constexpr FixedCharArray(const C (&str)[N]) noexcept {

        for (SizeT i = 0; i < N; i++) {
            data[i] = str[i];
        }

    }

    CharT data[N];

};