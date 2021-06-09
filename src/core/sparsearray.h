#pragma once

#include <vector>
#include "types.h"
#include "util/assert.h"
#include "util/optionalref.h"



template<class T, class IndexType = u32>
class SparseArray {

#ifdef ARC_SPARSE_PACK
    struct Storage {
        IndexType index;
        T element;
    };
#endif

public:

    constexpr static inline IndexType invalidIndex = -1;


    SparseArray() = default;

    SparseArray(IndexType indexArrayCapacity, IndexType denseArrayCapacity) {
        reserve(indexArrayCapacity, denseArrayCapacity);
    }


    /*
        Adds value to the array at position.
        If an element already exists at position, no operation is performed.
        The container is resized if position exceeds the current size.
        Returns true if the element has been added, false otherwise.
    */
    bool add(IndexType position, T&& value) {

        checkResize(position);
        const IndexType& index = indexArray[position];

        if(!containsValidElement(position, index)) {

            internalAdd(position, std::forward<T>(value));
            return true;

        }

        return false;

    }


    /*
        Sets the element at position to value.
        If an element already exists at position, the element is overwritten.
        The container is resized if position exceeds the current size.
    */
    void set(IndexType position, T&& value) {

        checkResize(position);
        const IndexType& index = indexArray[position];

        if(!containsValidElement(position, index)) {

            internalAdd(position, std::forward<T>(value));

        } else {

            internalSet(position, index, std::forward<T>(value));

        }

    }


     /*
        Sets the element at position to value.
        If an element does not already exist, no operation is performed.
        The container is resized if position exceeds the current size.
    */
    bool trySet(IndexType position, T&& value) {

        checkResize(position);
        IndexType& index = indexArray[position];

        if(containsValidElement(position, index)) {

            internalSet(position, index, std::forward<T>(value));
            return true;

        }

        return false;

    }


    /*
        Returns whether an element at a given position exists.
    */
    bool contains(IndexType position) {
        return !sparseIndexOutOfBounds(position) && containsValidElement(position, indexArray[position]);
    }


    /*
        Returns an optional holding a reference to the requested element or nothing if the given index does not map to a valid element.
    */
    OptionalRef<T> tryGet(IndexType position) {

        if(!contains(position)) {
            return {};
        }

        return internalGet(position);

    }


    /*
        Returns a reference to the element at the given position. This function exhibits UB if no such element exists.
    */
    T& get(IndexType position) {
        return internalGet(position);
    }


    /*
        Attempts to remove the given object. Returns true if such object existed and has been deleted, false otherwise.
    */
    bool tryRemove(IndexType position) {

        if(contains(position)) {

            internalRemove(position);
            return true;

        }

        return false;

    }


    /*
        Removes the given object. UB is exhibited in case the element does not exist.
    */
    void remove(IndexType position) {
        internalRemove(position);
    }


    /*
        Returns the number of sparse indices in the array.
    */
    u32 getSparseSize() const {
        return indexArray.size();
    }


    /*
        Returns the number of elements in the array.
    */
    u32 getSize() const {
        return denseArray.size();
    }


    /*
        Reserves the given minimum amount of memory for both arrays.
    */
    void reserve(u32 indexArrayCapacity, u32 denseArrayCapacity) {

        indexArray.reserve(indexArrayCapacity);
        denseArray.reserve(denseArrayCapacity);
#ifndef ARC_SPARSE_PACK
        elementArray.reserve(denseArrayCapacity);
#endif

    }


    /*
        Swaps the elements at posA and posB.
    */
    void swap(u32 posA, u32 posB) {
        std::swap(indexArray[posA], indexArray[posB]);
    }


private:

    /*
        Fully swaps both sparse and dense indices
    */
    void exchange(u32 posA, u32 posB) {

        swap(posA, posB);
        std::swap(indexArray[posB], indexArray[posA]);

    }


    void internalSet(IndexType sparseIdx, IndexType denseIdx, T&& value) {
#ifdef ARC_SPARSE_ARRAY
        denseArray[denseIdx].index = sparseIdx;
        denseArray[denseIdx].storage = value;
#else
        denseArray[denseIdx] = sparseIdx;
        elementArray[denseIdx] = value;
#endif
    }


    T& internalGet(IndexType sparseIdx) {
#ifdef ARC_SPARSE_PACK
        return denseArray[indexArray[sparseIdx]].storage;
#else
        return elementArray[indexArray[sparseIdx]];
#endif
    }


    void internalAdd(IndexType sparseIdx, T&& value) {

        indexArray[sparseIdx] = denseArray.size();

#ifdef ARC_SPARSE_PACK
        denseArray.push_back(Storage{sparseIdx, value});
#else
        denseArray.push_back(sparseIdx);
        elementArray.push_back(value);
#endif

    }


    void internalRemove(IndexType sparseIdx) {

#ifdef ARC_SPARSE_PACK
        const IndexType& lastSparseIdx = denseArray.back().index;
        const IndexType& denseIdx = indexArray[sparseIdx];
        denseArray[denseIdx] = std::move(denseArray[indexArray[lastSparseIdx]]);

        indexArray[sparseIdx] = invalidIndex;
        indexArray[lastSparseIdx] = denseIdx;

        auto newSize = denseArray.size() - 1;
        denseArray.resize(newSize);
#else
        const IndexType& lastSparseIdx = denseArray.back();
        const IndexType& denseIdx = indexArray[sparseIdx];
        denseArray[denseIdx] = std::move(denseArray[indexArray[lastSparseIdx]]);
        elementArray[denseIdx] = std::move(denseArray[indexArray[lastSparseIdx]]);

        indexArray[sparseIdx] = invalidIndex;
        indexArray[lastSparseIdx] = denseIdx;

        auto newSize = denseArray.size() - 1;
        denseArray.resize(newSize);
        elementArray.resize(newSize);
#endif

    }


    /*
        Checks if the requested position is greater-equal than the current size.
        If yes, the index array is resized and filled with invalid indices.
    */
    void checkResize(IndexType reqPos) {

        if(reqPos >= indexArray.size()) {
            indexArray.resize(reqPos + 1, invalidIndex);
        }

    }


    /*
        Returns true if a valid element exists at the given indices
    */
    bool containsValidElement(IndexType sparseIdx, IndexType denseIdx) {
        return !denseIndexInvalid(denseIdx) && !denseElementInvalid(sparseIdx);
    }


    /*
        Returns true if the sparse index is out of bounds
    */
    bool sparseIndexOutOfBounds(IndexType sparseIdx) {
        return sparseIdx >= indexArray.size();
    }

   
    /*
        Returns true if the dense index is out of bounds
    */
    bool denseIndexOutOfBounds(IndexType denseIdx) {
        return denseIdx >= denseArray.size();
    }


    /*
        Returns true if the dense index is invalid
    */
    bool denseIndexInvalid(IndexType denseIdx) {
        return denseIdx == invalidIndex;
    }


    /*
        Returns true if the dense element is invalid
    */
    bool denseElementInvalid(IndexType sparseIdx) {
#ifdef ARC_SPARSE_PACK
        return denseArray[indexArray[sparseIdx]].index != sparseIdx;
#else
        return denseArray[indexArray[sparseIdx]] != sparseIdx;
#endif
    }

    
    /*
        Asserts if the given sparse index is out of bounds.
    */
    void checkSparseIndexOutOfBounds(IndexType pos) {
        arc_assert(!indexOutOfBounds(pos), "Sparse index out of bounds (idx=%d, size=%d)", pos, indexArray.size());
    }


    /*
        Asserts if the given dense index is out of bounds.
    */
    void checkDenseIndexOutOfBounds(IndexType pos) {
        arc_assert(!denseIndexOutOfBounds(pos), "Dense index out of bounds (idx=%d, size=%d)", pos, denseArray.size());
    }

#ifndef ARC_SPARSE_PACK
    std::vector<T> elementArray;
    std::vector<IndexType> denseArray;
#else
    std::vector<Storage> denseArray;
#endif

    std::vector<IndexType> indexArray;

};