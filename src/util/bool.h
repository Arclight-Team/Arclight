#pragma once

#include "util/concepts.h"
#include "types.h"



namespace Bool {

    template<class T, class... Compare> requires ((Equal<std::remove_cvref_t<T>, std::remove_cvref_t<Compare>> && ...) || (Convertible<Compare, T> && ...))
    constexpr bool any(T obj, Compare&&... comp) {
        return ((obj == comp) || ...);
    }

    template<class T, class... Compare> requires ((Equal<std::remove_cvref_t<T>, std::remove_cvref_t<Compare>> && ...) || (Convertible<Compare, T> && ...))
    constexpr bool none(T obj, Compare&&... comp) {
        return ((obj != comp) && ...);
    }

    template<SizeT N, class T, class... Compare> requires ((Equal<std::remove_cvref_t<T>, std::remove_cvref_t<Compare>> && ...) || (Convertible<Compare, T> && ...))
    constexpr bool n(T obj, Compare&&... comp) {
        return ((obj == comp) + ...) == N;
    }

    template<class T, class... Compare> requires ((Equal<std::remove_cvref_t<T>, std::remove_cvref_t<Compare>> && ...) || (Convertible<Compare, T> && ...))
    constexpr bool one(T&& obj, Compare&&... comp) {
        return n<1>(std::forward<T>(obj), std::forward<Compare>(comp)...);
    }

    template<class T, class... Compare> requires ((Equal<std::remove_cvref_t<T>, std::remove_cvref_t<Compare>> && ...) || (Convertible<Compare, T> && ...))
    constexpr bool all(T&& obj, Compare&&... comp) {
        return ((obj == comp) && ...);
    }

}