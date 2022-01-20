/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 typetraits.hpp
 */

#pragma once

#include "types.hpp"

#include <type_traits>


namespace TT {

    template<class T, class... Comp>
    struct _IsAnyOf {
        constexpr static bool Value = (std::is_same_v<T, Comp> || ...);
    };

    template<class T, class... Comp>
    constexpr inline bool IsAnyOf = _IsAnyOf<T, Comp...>::Value;

    template<class T, class... Pack>
    struct _IsAllSame {
        constexpr static bool Value = (std::is_same_v<T, Pack> && ...);
    };

    template<class T, class... Pack>
    constexpr inline bool IsAllSame = _IsAllSame<T, Pack...>::Value;

    template<SizeT N, class... Pack> requires (N < sizeof...(Pack))
    constexpr inline SizeT SizeofN = []() { 

        SizeT i = 0;
        SizeT s = 0;
        [[maybe_unused]] bool x = ((i++ == N ? (s = sizeof(Pack), true) : false) || ...);
        
        return s;

    }();

	template<bool B, class T>
	using ConditionalConst = std::conditional_t<B, const T, T>;

    template<Float F>
    struct _ToInteger {
        using Type = i32;
    };

    template<>
    struct _ToInteger<double> {
        using Type = i64;
    };

    template<>
    struct _ToInteger<long double> {
        using Type = i64;
    };

    template<class T>
    using ToInteger = _ToInteger<T>::Type;


    template<Integer I>
    struct _ToFloat {
        using Type = float;
    };

    template<>
    struct _ToFloat<u64> {
        using Type = double;
    };

    template<>
    struct _ToFloat<i64> {
        using Type = double;
    };

    template<class T>
    using ToFloat = _ToFloat<T>::Type;

}

