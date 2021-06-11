#pragma once

#include <type_traits>


namespace TypeTraits {

    template<class T, class... Comp>
    struct IsAnyOf {
        constexpr static bool value = std::disjunction_v<std::is_same<T, Comp>...>;
    };

    template<class T, class... Comp>
    constexpr inline bool IsAnyOfV = IsAnyOf<T, Comp...>::value;

}

