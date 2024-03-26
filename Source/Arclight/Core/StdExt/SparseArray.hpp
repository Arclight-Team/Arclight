/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SparseArray.hpp
 */

#pragma once

#include "STDExt/OptionalRef.hpp"
#include "Common/Assert.hpp"
#include "Meta/TypeTraits.hpp"
#include "Common/Config.hpp"
#include "Common/Types.hpp"

#include <vector>
#include <iterator>



/*
	Sparse Array implementation with reverse lookup

	Complexity for lookup, insertion, deletion is O(1). Traversal is O(n).
	Memory overhead is proportional to the number of sparse entries.
*/
template<class T, class IndexType = u32>
class SparseArray {

	static_assert(!std::is_reference_v<T>, "T cannot be a reference type");

#ifdef ARC_CFG_SPARSE_PACK
	struct Storage {
		IndexType index;
		T element;
	};
#endif

	template<bool Const>
	class IteratorBase {

	public:
	
		using iterator_category = std::contiguous_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using value_type        = TT::ConditionalConst<Const, T>;
		using pointer           = value_type*;
		using reference         = value_type&;
		using element_type      = value_type;


		constexpr IteratorBase() noexcept : ptr(nullptr) {}

#ifdef ARC_CFG_SPARSE_PACK
		constexpr IteratorBase(Storage* it) noexcept : ptr(it) {}
#else
		constexpr explicit IteratorBase(pointer it) noexcept : ptr(it) {}
#endif

		template<bool ConstOther> requires (Const >= ConstOther)
		constexpr explicit IteratorBase(const IteratorBase<ConstOther>& it) noexcept : Iterator(it.ptr) {}

		constexpr reference operator*() const noexcept {return *getPtr();}
		constexpr pointer operator->() const noexcept {return getPtr();}
		constexpr reference operator[](SizeT n) const noexcept {return *(*this + n);}

		constexpr IteratorBase& operator++() noexcept {ptr++; return *this;}
		constexpr IteratorBase operator++(int) noexcept {IteratorBase cpy = *this; ++(*this); return cpy;}
		constexpr IteratorBase& operator--() noexcept {ptr--; return *this;}
		constexpr IteratorBase operator--(int) noexcept {IteratorBase cpy = *this; --(*this); return cpy;}

		constexpr IteratorBase operator+(SizeT n) const noexcept {return IteratorBase(ptr + n);}
		constexpr IteratorBase operator-(SizeT n) const noexcept {return IteratorBase(ptr - n);}
		constexpr IteratorBase& operator+=(SizeT n) noexcept {ptr += n; return *this;}
		constexpr IteratorBase& operator-=(SizeT n) noexcept {ptr -= n; return *this;}

		friend constexpr IteratorBase operator+(SizeT n, const IteratorBase& it) noexcept { return it + n; }

		template<bool ConstOther>
		constexpr difference_type operator-(const IteratorBase<ConstOther>& other) const noexcept {return ptr - other.ptr;}

		template<bool ConstOther>
		constexpr auto operator<=>(const IteratorBase<ConstOther>& other) const noexcept {
			return ptr <=> other.ptr;
		}

		template<bool ConstOther>
		constexpr auto operator==(const IteratorBase<ConstOther>& other) const noexcept {
			return ptr == other.ptr;
		}

	private:

		template<bool ConstOther>
		friend class IteratorBase;

#ifdef ARC_CFG_SPARSE_PACK
		constexpr pointer getPtr() const noexcept {return &ptr->element;}
		Storage* ptr;
#else
		constexpr pointer getPtr() const noexcept {return ptr;}
		pointer ptr;
#endif

	};

public:

	using Iterator = IteratorBase<false>;
	using ConstIterator = IteratorBase<true>;

	using ReverseIterator = std::reverse_iterator<Iterator>;
	using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

	constexpr static IndexType invalidIndex = -1;


	constexpr SparseArray() noexcept = default;

	constexpr SparseArray(IndexType indexArrayCapacity, IndexType denseArrayCapacity) {
		reserve(indexArrayCapacity, denseArrayCapacity);
	}


	/*
		Adds value to the array at position.
		If an element already exists at position, no operation is performed.
		The container is resized if position exceeds the current size.
		Returns true if the element has been added, false otherwise.
	*/
	constexpr bool add(IndexType position, const T& value) {

		checkResize(position);
		const IndexType& index = indexArray[position];

		if(!containsValidElement(position, index)) {

			internalAdd(position, value);
			return true;

		}

		return false;

	}

	constexpr bool add(IndexType position, T&& value) {

		checkResize(position);
		const IndexType& index = indexArray[position];

		if(!containsValidElement(position, index)) {

			internalAdd(position, std::move(value));
			return true;

		}

		return false;

	}


	/*
		Sets the element at position to value.
		If an element already exists at position, the element is overwritten.
		The container is resized if position exceeds the current size.
	*/
	constexpr void set(IndexType position, const T& value) {

		checkResize(position);
		const IndexType& index = indexArray[position];

		if(!containsValidElement(position, index)) {

			internalAdd(position, value);

		} else {

			internalSet(position, index, value);

		}

	}

	constexpr void set(IndexType position, T&& value) {

		checkResize(position);
		const IndexType& index = indexArray[position];

		if(!containsValidElement(position, index)) {

			internalAdd(position, std::move(value));

		} else {

			internalSet(position, index, std::move(value));

		}

	}


	/*
		Sets the element at position to value.
		If an element does not already exist, no operation is performed.
		The container is resized if position exceeds the current size.
	*/
	constexpr bool trySet(IndexType position, const T& value) {

		checkResize(position);
		IndexType& index = indexArray[position];

		if(containsValidElement(position, index)) {

			internalSet(position, index, value);
			return true;

		}

		return false;

	}

	constexpr bool trySet(IndexType position, T&& value) {

		checkResize(position);
		IndexType& index = indexArray[position];

		if(containsValidElement(position, index)) {

			internalSet(position, index, std::move(value));
			return true;

		}

		return false;

	}


	/*
		Returns whether an element at a given position exists.
	*/
	constexpr bool contains(IndexType position) const {
		return !sparseIndexOutOfBounds(position) && containsValidElement(position, indexArray[position]);
	}


	/*
		Returns an optional holding a reference to the requested element or nothing if the given index does not map to a valid element.
	*/
	constexpr OptionalRef<T> tryGet(IndexType position) {

		if(!contains(position)) {
			return {};
		}

		return internalGet(position);

	}


	constexpr OptionalRef<const T> tryGet(IndexType position) const {

		if(!contains(position)) {
			return {};
		}

		return internalGet(position);

	}


	/*
		Returns a reference to the element at the given position. These functions exhibits UB if no such element exists.
	*/
	constexpr T& get(IndexType position) {
		return internalGet(position);
	}

		
	constexpr const T& get(IndexType position) const {
		return internalGet(position);
	}


	constexpr T& operator[](SizeT n) {
		return get(n);
	}


	constexpr const T& operator[](SizeT n) const {
		return get(n);
	}


	/*
		Returns the corresponding sparse index to the iterator. These functions exhibits UB if no such element exists.
	*/
	constexpr IndexType invert(const Iterator& iter) const {
		return internalInvertIndex(iter - begin());
	}

	constexpr IndexType invert(const ConstIterator& iter) const {
		return internalInvertIndex(iter - cbegin());
	}

	constexpr IndexType invert(const ReverseIterator& iter) const {
		return internalInvertIndex(getSize() - iter + rbegin());
	}

	constexpr IndexType invert(const ConstReverseIterator& iter) const {
		return internalInvertIndex(getSize() - iter + crbegin());
	}


	/*
		Attempts to remove the given object. Returns true if such object existed and has been deleted, false otherwise.
	*/
	constexpr bool tryRemove(IndexType position) {

		if(contains(position)) {

			internalRemove(position);
			return true;

		}

		return false;

	}


	/*
		Removes the given object. UB is exhibited in case the element does not exist.
	*/
	constexpr void remove(IndexType position) {
		internalRemove(position);
	}


	/*
		Clears the container. All elements will be removed.
	*/
	constexpr void clear() noexcept {

		indexArray.clear();
		denseArray.clear();

#ifndef ARC_CFG_SPARSE_PACK
		elementArray.clear();
#endif

	}


	/*
		Resets the container. All elements are removed and memory is requested to be deallocated.
	*/
	constexpr void reset() {

		clear();

		indexArray.shrink_to_fit();
		denseArray.shrink_to_fit();

#ifndef ARC_CFG_SPARSE_PACK
		elementArray.shrink_to_fit();
#endif

	}


	/*
		Returns the number of sparse indices in the array.
	*/
	constexpr SizeT getSparseSize() const noexcept {
		return indexArray.size();
	}


	/*
		Returns the number of elements in the array.
	*/
	constexpr SizeT getSize() const noexcept {
		return denseArray.size();
	}


	/*
		Returns an iterator to the start of the dense array.
	*/
	constexpr Iterator begin() noexcept {
#ifdef ARC_CFG_SPARSE_PACK
		return Iterator(denseArray.data());
#else
		return Iterator(elementArray.data());
#endif
	}


	constexpr ConstIterator begin() const noexcept {
#ifdef ARC_CFG_SPARSE_PACK
		return ConstIterator(denseArray.data());
#else
		return ConstIterator(elementArray.data());
#endif
	}


	/*
		Returns an iterator to the element past the dense array.
	*/
	constexpr Iterator end() noexcept {
#ifdef ARC_CFG_SPARSE_PACK
		return Iterator(denseArray.data() + denseArray.size());
#else
		return Iterator(elementArray.data() + elementArray.size());
#endif
	}


	constexpr ConstIterator end() const noexcept {
#ifdef ARC_CFG_SPARSE_PACK
		return ConstIterator(denseArray.data() + denseArray.size());
#else
		return ConstIterator(elementArray.data() + elementArray.size());
#endif
	}


	/*
		Returns a const iterator to the start of the dense array.
	*/
	constexpr ConstIterator cbegin() const noexcept {
#ifdef ARC_CFG_SPARSE_PACK
		return ConstIterator(denseArray.data());
#else
		return ConstIterator(elementArray.data());
#endif
	}


	/*
		Returns a const iterator to the element past the dense array.
	*/
	constexpr ConstIterator cend() const noexcept {
#ifdef ARC_CFG_SPARSE_PACK
		return ConstIterator(denseArray.data() + denseArray.size());
#else
		return ConstIterator(elementArray.data() + elementArray.size());
#endif
	}


	/*
		Returns a reverse iterator to the start of the dense array.
	*/
	constexpr ReverseIterator rbegin() noexcept {
		return ReverseIterator(end());
	}


	constexpr ConstReverseIterator rbegin() const noexcept {
		return ConstReverseIterator(end());
	}


	/*
		Returns a reverse iterator to the element past the dense array.
	*/
	constexpr ReverseIterator rend() noexcept {
		return ReverseIterator(begin());
	}


	constexpr ConstReverseIterator rend() const noexcept {
		return ConstReverseIterator(begin());
	}


	/*
		Returns a const reverse iterator to the start of the dense array.
	*/
	constexpr ConstReverseIterator crbegin() const noexcept {
		return ConstReverseIterator(cend());
	}


	/*
		Returns a const reverse iterator to the element past the dense array.
	*/
	constexpr ConstReverseIterator crend() const noexcept {
		return ConstReverseIterator(cbegin());
	}


	/*
		Searches through the dense array until it finds the first object equal to compare.
		If found, the corresponding index is returned.
		If not, the invalid index is returned.
		Note that this operation has a complexity of O(n).
	*/
	constexpr IndexType find(const T& compare) const {

		for(SizeT i = 0; i < denseArray.size(); i++){
			
#ifdef ARC_CFG_SPARSE_PACK
			if(denseArray[i].storage == compare) {
				return denseArray[i].index;
			}
#else
			if(elementArray[i] == compare) {
				return denseArray[i];
			}
#endif

		}

		return invalidIndex;

	}



	/*
		Reserves the given minimum amount of memory for both arrays.
	*/
	constexpr void reserve(IndexType indexArrayCapacity, IndexType denseArrayCapacity) {

		indexArray.reserve(indexArrayCapacity);
		denseArray.reserve(denseArrayCapacity);
#ifndef ARC_CFG_SPARSE_PACK
		elementArray.reserve(denseArrayCapacity);
#endif

	}


	/*
		Swaps the elements at posA and posB.
	*/
	constexpr void swap(IndexType posA, IndexType posB) {

#ifdef ARC_CFG_SPARSE_PACK
		std::swap(denseArray[indexArray[posA]].index, denseArray[indexArray[posB]].index);
#else
		std::swap(denseArray[indexArray[posA]], denseArray[indexArray[posB]]);
#endif
		std::swap(indexArray[posA], indexArray[posB]);

	}


private:

	template<class U>
	constexpr void internalSet(IndexType sparseIdx, IndexType denseIdx, U&& value) {
#ifdef ARC_CFG_SPARSE_PACK
		denseArray[denseIdx].index = sparseIdx;
		denseArray[denseIdx].element = std::forward<U>(value);
#else
		denseArray[denseIdx] = sparseIdx;
		elementArray[denseIdx] = std::forward<U>(value);
#endif
	}


	constexpr T& internalGet(IndexType sparseIdx) {
#ifdef ARC_CFG_SPARSE_PACK
		return denseArray[indexArray[sparseIdx]].element;
#else
		return elementArray[indexArray[sparseIdx]];
#endif
	}


	constexpr const T& internalGet(IndexType sparseIdx) const {
#ifdef ARC_CFG_SPARSE_PACK
		return denseArray[indexArray[sparseIdx]].element;
#else
		return elementArray[indexArray[sparseIdx]];
#endif
	}


	template<class U>
	constexpr void internalAdd(IndexType sparseIdx, U&& value) {

		indexArray[sparseIdx] = denseArray.size();

#ifdef ARC_CFG_SPARSE_PACK
		denseArray.emplace_back(Storage{sparseIdx, std::forward<U>(value)});
#else
		denseArray.emplace_back(sparseIdx);
		elementArray.emplace_back(std::forward<U>(value));
#endif

	}


	constexpr void internalRemove(IndexType sparseIdx) {

#ifdef ARC_CFG_SPARSE_PACK
		const IndexType& lastSparseIdx = denseArray.back().index;
		const IndexType& denseIdx = indexArray[sparseIdx];
		denseArray[denseIdx] = std::move(denseArray[indexArray[lastSparseIdx]]);

		indexArray[lastSparseIdx] = denseIdx;
		indexArray[sparseIdx] = invalidIndex;

		denseArray.pop_back();
#else
		const IndexType& lastSparseIdx = denseArray.back();
		const IndexType& denseIdx = indexArray[sparseIdx];
		denseArray[denseIdx] = std::move(denseArray[indexArray[lastSparseIdx]]);
		elementArray[denseIdx] = std::move(elementArray[indexArray[lastSparseIdx]]);

		indexArray[lastSparseIdx] = denseIdx;
		indexArray[sparseIdx] = invalidIndex;

		denseArray.pop_back();
		elementArray.pop_back();
#endif

	}


	constexpr IndexType internalInvertIndex(IndexType denseIdx) const {

#ifdef ARC_CFG_SPARSE_PACK
		return denseArray[denseIdx].index;
#else
		return denseArray[denseIdx];
#endif

	}


	/*
		Checks if the requested position is greater-equal than the current size.
		If yes, the index array is resized and filled with invalid indices.
	*/
	constexpr void checkResize(IndexType reqPos) {

		arc_assert(reqPos <= 0xFFFFFF, "Index %d exceeds the usual array range, is your index correct?", reqPos);

		if(reqPos >= indexArray.size()) {
			 indexArray.resize(reqPos + 1, invalidIndex);
		}

	}


	/*
		Returns true if a valid element exists at the given indices
	*/
	constexpr bool containsValidElement(IndexType sparseIdx, IndexType denseIdx) const noexcept {
		return !denseIndexInvalid(denseIdx) && !denseElementInvalid(sparseIdx);
	}


	/*
		Returns true if the sparse index is out of bounds
	*/
	constexpr bool sparseIndexOutOfBounds(IndexType sparseIdx) const noexcept {
		return sparseIdx >= indexArray.size();
	}

   
	/*
		Returns true if the dense index is out of bounds
	*/
	constexpr bool denseIndexOutOfBounds(IndexType denseIdx) const noexcept {
		return denseIdx >= denseArray.size();
	}


	/*
		Returns true if the dense index is invalid
	*/
	constexpr bool denseIndexInvalid(IndexType denseIdx) const noexcept {
		return denseIdx == invalidIndex;
	}


	/*
		Returns true if the dense element is invalid
	*/
	constexpr bool denseElementInvalid(IndexType sparseIdx) const {
#ifdef ARC_CFG_SPARSE_PACK
		return denseArray[indexArray[sparseIdx]].index != sparseIdx;
#else
		return denseArray[indexArray[sparseIdx]] != sparseIdx;
#endif
	}

	
	/*
		Asserts if the given sparse index is out of bounds.
	*/
	constexpr void checkSparseIndexOutOfBounds(IndexType pos) const noexcept {
		arc_assert(!sparseIndexOutOfBounds(pos), "Sparse index out of bounds (idx=%d, size=%d)", pos, indexArray.size());
	}


	/*
		Asserts if the given dense index is out of bounds.
	*/
	constexpr void checkDenseIndexOutOfBounds(IndexType pos) const noexcept {
		arc_assert(!denseIndexOutOfBounds(pos), "Dense index out of bounds (idx=%d, size=%d)", pos, denseArray.size());
	}

#ifndef ARC_CFG_SPARSE_PACK
	std::vector<T> elementArray;
	std::vector<IndexType> denseArray;
#else
	std::vector<Storage> denseArray;
#endif

	std::vector<IndexType> indexArray;

};