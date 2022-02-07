/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 AlignedAllocator.hpp
 */

#pragma once

#include "util/bits.hpp"
#include "math/math.hpp"
#include "types.hpp"

#include <new>
#include <limits>
#include <memory>
#include <stdexcept>



template<class T, AlignT ReqAlignment> requires (!ConstType<T>)
class AlignedAllocator {

public:

	constexpr static AlignT Alignment = Math::max(alignof(T), Bits::ceilPowerOf2(ReqAlignment));

	using value_type = T;
	using size_type = SizeT;
	using difference_type = std::ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;


	constexpr AlignedAllocator() noexcept = default;
	constexpr ~AlignedAllocator() noexcept = default;

	constexpr AlignedAllocator(const AlignedAllocator& alloc) noexcept = default;
	constexpr AlignedAllocator& operator=(const AlignedAllocator& alloc) = default;

	template<class U, AlignT ReqAlign>
	constexpr AlignedAllocator(const AlignedAllocator<U, ReqAlign>& alloc) noexcept {};


	[[nodiscard]] constexpr T* allocate(SizeT n) {

		if (std::numeric_limits<SizeT>::max() / sizeof(T) < n) {
			throw std::bad_array_new_length();
		}

		return static_cast<T*>(::operator new(n * sizeof(T), std::align_val_t(Alignment)));

	}

	constexpr void deallocate(T* p, [[maybe_unused]] SizeT n) noexcept {
		::operator delete(p, std::align_val_t(Alignment));
	}


	template<class U>
	struct rebind {
		using other = AlignedAllocator<U, ReqAlignment>;
	};

};


template<class T, AlignT A, class U, AlignT B>
constexpr bool operator==(const AlignedAllocator<T, A>& lhs, const AlignedAllocator<U, B>& rhs) noexcept {
	return true;
}