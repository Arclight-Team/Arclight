/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Bool.hpp
 */

#pragma once

#include "Meta/TypeTraits.hpp"
#include "Meta/Concepts.hpp"
#include "Common/Types.hpp"



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