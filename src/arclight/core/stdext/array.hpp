/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 array.hpp
 */

#pragma once

#include "types.hpp"
#include "common/typetraits.hpp"


template<class T, SizeT... N> requires(sizeof...(N) > 0)
class Array {

private:

	template<SizeT C, SizeT S, SizeT... Rest> requires(C <= sizeof...(Rest))
	struct ArraySlice {
		using Type = typename ArraySlice<C - 1, Rest...>::Type;
	};

	template<SizeT S, SizeT... Rest>
	struct ArraySlice<0, S, Rest...> {
		using Type = Array<T, Rest...>;
	};

	template<SizeT S>
	struct ArraySlice<0, S> {
		using Type = T;
	};


	template<SizeT S, SizeT... Rest>
	struct ArrayStorage {
		using Type = typename ArraySlice<0, S, Rest...>::Type[S];
	};

public:

	// Slices the current array type recursively (e.g. Array<T, N, N> -> Array<T, N> -> T)
	template<SizeT C>
	using SliceT = typename ArraySlice<C, N...>::Type;

	using StorageT = typename ArrayStorage<N...>::Type;

	using Type = T;


	static constexpr SizeT Dimensions = sizeof...(N);

	static constexpr SizeT TotalSize = (N * ...);

	static constexpr SizeT DimensionSize[Dimensions] = {N...};

	static constexpr SizeT SliceSize = DimensionSize[0];

private:

	template<bool Const>
	class IteratorBase {

	public:

		using iterator_category = std::contiguous_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using value_type		= TT::ConditionalConst<Const, SliceT<0>>;
		using pointer			= value_type*;
		using reference			= value_type&;


		constexpr IteratorBase() noexcept : ptr(nullptr) {}

		constexpr explicit IteratorBase(pointer ptr) noexcept : ptr(ptr) {}


		constexpr reference operator*() const noexcept { return *ptr; }
		constexpr reference operator[](SizeT n) const noexcept { return *(*this + n); }
		constexpr pointer operator->() const noexcept { return ptr; }

		constexpr IteratorBase& operator++() noexcept { ptr++; return *this; }
		constexpr IteratorBase& operator--() noexcept { ptr--; return *this; }

		constexpr IteratorBase operator++(int) noexcept { IteratorBase tmp = *this; ++(*this); return tmp; }
		constexpr IteratorBase operator--(int) noexcept { IteratorBase tmp = *this; --(*this); return tmp; }

		constexpr IteratorBase& operator+=(SizeT n) noexcept { ptr += n; return *this; }
		constexpr IteratorBase& operator-=(SizeT n) noexcept { ptr -= n; return *this; }

		constexpr IteratorBase operator+(SizeT n) const noexcept { return IteratorBase(ptr + n); }
		constexpr IteratorBase operator-(SizeT n) const noexcept { return IteratorBase(ptr - n); }

		friend constexpr IteratorBase operator+(SizeT n, const IteratorBase& it) { return it + n; }

		template<bool C>
		constexpr difference_type operator-(const IteratorBase<C>& it) const noexcept { return ptr - it.ptr; }


		template<bool C>
		constexpr bool operator==(const IteratorBase<C>& it) const noexcept { return ptr == it.ptr; }

		template<bool C>
		constexpr auto operator<=>(const IteratorBase<C>& it) const noexcept { return ptr <=> it.ptr; }

	private:

		template<bool C>
		friend class IteratorBase;


		pointer ptr;

	};

public:

	using Iterator				= IteratorBase<false>;
	using ConstIterator			= IteratorBase<true>;
	using ReverseIterator		= std::reverse_iterator<Iterator>;
	using ConstReverseIterator	= std::reverse_iterator<ConstIterator>;


	template<CC::ImpConvertible<SizeT>... Args> requires(sizeof...(Args) <= Dimensions)
	constexpr SliceT<sizeof...(Args) - 1>& at(Args... index) noexcept {
		return accessHelper(index...);
	}

	template<CC::ImpConvertible<SizeT>... Args> requires(sizeof...(Args) <= Dimensions)
	constexpr const SliceT<sizeof...(Args) - 1>& at(Args... index) const noexcept {
		return accessHelper(index...);
	}


	// Temporary alternative for multidimensional subscript (C++23)
	constexpr auto& operator()(auto... index) noexcept {
		return at(index...);
	}

	constexpr const auto& operator()(auto... index) const noexcept {
		return at(index...);
	}

	constexpr SliceT<0>& operator[](SizeT index) noexcept {
		return at(index);
	}

	constexpr const SliceT<0>& operator[](SizeT index) const noexcept {
		return at(index);
	}


	constexpr Type& planarFront() noexcept {
		return at((N * 0)...);
	}

	constexpr const Type& planarFront() const noexcept {
		return at((N * 0)...);
	}


	constexpr Type& planarBack() noexcept {
		return at((N - 1)...);
	}

	constexpr const Type& planarBack() const noexcept {
		return at((N - 1)...);
	}


	constexpr Type* planarData() noexcept {
		return &planarFront();
	}

	constexpr const Type* planarData() const noexcept {
		return &planarFront();
	}


	constexpr SliceT<0>& front() noexcept {
		return at(0);
	}

	constexpr const SliceT<0>& front() const noexcept {
		return at(0);
	}


	constexpr SliceT<0>& back() noexcept {
		return at(SliceSize);
	}

	constexpr const SliceT<0>& back() const noexcept {
		return at(SliceSize);
	}


	constexpr SliceT<0>* data() noexcept {
		return storage;
	}

	constexpr const SliceT<0>* data() const noexcept {
		return storage;
	}


	constexpr bool empty() const noexcept {
		return SliceSize == 0;
	}

	constexpr bool totalEmpty() const noexcept {
		return TotalSize == 0;
	}


	constexpr void fill(const SliceT<0>& value) noexcept requires(Dimensions != 0) {
		for (auto& s : storage) {
			s = value;
		}
	}

	constexpr void fill(const Type& value) noexcept {
		for (auto& s : storage) {
			if constexpr (Dimensions == 1) {
				s = value;
			} else {
				s.fill(value);
			}
		}
	}


	constexpr SizeT size() const noexcept {
		return SliceSize;
	}

	constexpr SizeT totalSize() const noexcept {
		return TotalSize;
	}

	constexpr SizeT dimensionSize(SizeT dimension) const noexcept {
		arc_assert(dimension < Dimensions, "Dimension out of range");
		return DimensionSize[dimension];
	}


	constexpr Iterator begin() noexcept {
		return Iterator(data());
	}

	constexpr ConstIterator begin() const noexcept {
		return ConstIterator(data());
	}

	constexpr ConstIterator cbegin() const noexcept {
		return ConstIterator(data());
	}


	constexpr Iterator end() noexcept {
		return Iterator(data() + size());
	}

	constexpr ConstIterator end() const noexcept {
		return ConstIterator(data() + size());
	}

	constexpr ConstIterator cend() const noexcept {
		return ConstIterator(data() + size());
	}


	constexpr ReverseIterator rbegin() noexcept {
		return ReverseIterator(end());
	}

	constexpr ConstReverseIterator rbegin() const noexcept {
		return ConstReverseIterator(end());
	}

	constexpr ConstReverseIterator crbegin() const noexcept {
		return ConstReverseIterator(end());
	}


	constexpr ReverseIterator rend() noexcept {
		return ReverseIterator(begin());
	}

	constexpr ConstReverseIterator rend() const noexcept {
		return ConstReverseIterator(begin());
	}

	constexpr ConstReverseIterator crend() const noexcept {
		return ConstReverseIterator(begin());
	}


	StorageT storage;

private:

	template<class A, SizeT... S> requires(sizeof...(S) > 0)
	friend class Array;

	template<CC::ImpConvertible<SizeT>... Args> requires(sizeof...(Args) <= Dimensions)
	constexpr SliceT<sizeof...(Args)>& accessHelper(SizeT index, Args... rest) noexcept {

		arc_assert(index < SliceSize, "Array access out of range");

		if constexpr (sizeof...(rest) != 0) {
			return storage[index].accessHelper(rest...);
		} else {
			return storage[index];
		}

	}

	template<CC::ImpConvertible<SizeT>... Args> requires(sizeof...(Args) <= Dimensions)
	constexpr const SliceT<sizeof...(Args)>& accessHelper(SizeT index, Args... rest) const noexcept {

		arc_assert(index < SliceSize, "Array access out of range");

		if constexpr (sizeof...(rest) != 0) {
			return storage[index].accessHelper(rest...);
		} else {
			return storage[index];
		}

	}

};


template<class A, CC::Equal<A>... B>
Array(A, B...) -> Array<A, sizeof...(B) + 1>;
