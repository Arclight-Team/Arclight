#pragma once

#include <type_traits>

#include "types.h"


namespace TT {

    template<class T, class... Comp>
    struct _IsAnyOf {
        constexpr static bool Value = std::disjunction_v<std::is_same<T, Comp>...>;
    };

    template<class T, class... Comp>
    constexpr inline bool IsAnyOf = _IsAnyOf<T, Comp...>::Value;

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

