/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 virtualallocator.hpp
 */

#pragma once

#include "virtualmemory.hpp"
#include "util/bits.hpp"
#include "math/math.hpp"
#include "common/types.hpp"

#include <new>
#include <limits>
#include <memory>
#include <stdexcept>



template<class T, VirtualMemory::Protection Protection> requires (!CC::ConstType<T>)
class VirtualAllocator {

public:

	using value_type = T;
	using size_type = SizeT;
	using difference_type = std::ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;


	constexpr VirtualAllocator() noexcept = default;
	constexpr ~VirtualAllocator() noexcept = default;

	constexpr VirtualAllocator(const VirtualAllocator& alloc) noexcept = default;
	constexpr VirtualAllocator& operator=(const VirtualAllocator& alloc) = default;

	template<class U, VirtualMemory::Protection P>
	constexpr VirtualAllocator(const VirtualAllocator<U, P>& alloc) noexcept {};


	[[nodiscard]] constexpr T* allocate(SizeT n) {

		if (std::numeric_limits<SizeT>::max() / sizeof(T) < n) {
			throw std::bad_array_new_length();
		}

		return static_cast<T*>(VirtualMemory::allocate(n * sizeof(T), Protection));

	}

	constexpr void deallocate(T* p, [[maybe_unused]] SizeT n) noexcept {
		VirtualMemory::deallocate(p);
	}


	template<class U>
	struct rebind {
		using other = VirtualAllocator<U, Protection>;
	};

};


template<class T, VirtualMemory::Protection P, class U, VirtualMemory::Protection Q>
constexpr bool operator==(const VirtualAllocator<T, P>& lhs, const VirtualAllocator<U, Q>& rhs) noexcept {
	return true;
}