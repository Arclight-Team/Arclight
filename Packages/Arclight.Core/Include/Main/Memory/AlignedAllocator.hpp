/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 AlignedAllocator.hpp
 */

#pragma once

#include "Util/Bits.hpp"
#include "Math/Math.hpp"
#include "Common/Types.hpp"

#include <new>
#include <limits>
#include <memory>



template<class T, AlignT ReqAlignment> requires (!CC::ConstType<T>)
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



template<class T> requires (!CC::ConstType<T>)
class DynamicAlignedAllocator {

public:

	using value_type = T;
	using size_type = SizeT;
	using difference_type = std::ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;
	using is_always_equal = std::true_type;


	constexpr DynamicAlignedAllocator() noexcept : DynamicAlignedAllocator(alignof(T)) {}
	constexpr ~DynamicAlignedAllocator() noexcept = default;

	constexpr DynamicAlignedAllocator(const DynamicAlignedAllocator& alloc) noexcept = default;
	constexpr DynamicAlignedAllocator& operator=(const DynamicAlignedAllocator& alloc) = default;

	constexpr explicit DynamicAlignedAllocator(AlignT align) noexcept : alignment(align) {}

	template<class U>
	constexpr DynamicAlignedAllocator(const DynamicAlignedAllocator<U>& alloc) noexcept : alignment(alloc.getAlignment()) {}


	[[nodiscard]] constexpr T* allocate(SizeT n) {

		if (std::numeric_limits<SizeT>::max() / sizeof(T) < n) {
			throw std::bad_array_new_length();
		}

		return static_cast<T*>(::operator new(n * sizeof(T), std::align_val_t(alignment)));

	}

	constexpr void deallocate(T* p, [[maybe_unused]] SizeT n) noexcept {
		::operator delete(p, std::align_val_t(alignment));
	}


	constexpr void setAlignment(AlignT align) noexcept {
		alignment = align;
	}


	constexpr AlignT getAlignment() const noexcept {
		return alignment;
	}


	template<class U>
	struct rebind {
		using other = DynamicAlignedAllocator<U>;
	};

private:

	AlignT alignment;

};


template<class T, class U>
constexpr bool operator==(const DynamicAlignedAllocator<T>& lhs, const DynamicAlignedAllocator<U>& rhs) noexcept {
	return CC::Equal<T, U> && lhs.getAlignment() == rhs.getAlignment();
}