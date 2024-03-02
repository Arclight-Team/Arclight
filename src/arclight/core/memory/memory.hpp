/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 memory.hpp
 */

#pragma once

#include "common/types.hpp"

#include <utility>



namespace Memory {

	template<class T, class... Args>
	T* construct(void* object, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...))) {
		return ::new(object) T(std::forward<Args>(args)...);
	}

	template<class T>
	void destroy(T* object) noexcept(noexcept(std::declval<T>().~T())) {
		object->~T();
	}

	template<class T>
	constexpr AddressT pointerAddress(const T* ptr) {
		return reinterpret_cast<AddressT>(ptr);
	}

	template<class T>
	constexpr AddressT referenceAddress(const T& ptr) {
		return address(std::addressof(ptr));
	}

}