/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bool.hpp
 */

#pragma once

#include "common/typetraits.hpp"
#include "common/concepts.hpp"
#include "common/types.hpp"



namespace Bool {

	template<class T, class... Compare> requires ((CC::Equal<TT::RemoveCVRef<T>, TT::RemoveCVRef<Compare>> && ...) || (CC::Convertible<Compare, TT::RemoveCVRef<T>> && ...))
	constexpr bool any(T obj, Compare&&... comp) noexcept((noexcept(obj != comp) && ...)) {
		return ((obj == comp) || ...);
	}

	template<class T, class... Compare> requires ((CC::Equal<TT::RemoveCVRef<T>, TT::RemoveCVRef<Compare>> && ...) || (CC::Convertible<Compare, TT::RemoveCVRef<T>> && ...))
	constexpr bool none(T obj, Compare&&... comp) noexcept((noexcept(obj != comp) && ...)) {
		return ((obj != comp) && ...);
	}

	template<SizeT N, class T, class... Compare> requires ((CC::Equal<TT::RemoveCVRef<T>, TT::RemoveCVRef<Compare>> && ...) || (CC::Convertible<Compare, TT::RemoveCVRef<T>> && ...))
	constexpr bool n(T obj, Compare&&... comp) noexcept((noexcept(obj != comp) && ...)) {
		return ((obj == comp) + ...) == N;
	}

	template<class T, class... Compare> requires ((CC::Equal<TT::RemoveCVRef<T>, TT::RemoveCVRef<Compare>> && ...) || (CC::Convertible<Compare, TT::RemoveCVRef<T>> && ...))
	constexpr bool one(T&& obj, Compare&&... comp) noexcept((noexcept(obj != comp) && ...)) {
		return n<1>(std::forward<T>(obj), std::forward<Compare>(comp)...);
	}

	template<class T, class... Compare> requires ((CC::Equal<TT::RemoveCVRef<T>, TT::RemoveCVRef<Compare>> && ...) || (CC::Convertible<Compare, TT::RemoveCVRef<T>> && ...))
	constexpr bool all(T&& obj, Compare&&... comp) noexcept((noexcept(obj != comp) && ...)) {
		return ((obj == comp) && ...);
	}

}