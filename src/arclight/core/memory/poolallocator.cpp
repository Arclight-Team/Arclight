/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 poolallocator.cpp
 */

#include "poolallocator.hpp"
#include "math/math.hpp"
#include "util/log.hpp"
#include "arcconfig.hpp"

#include <new>
#include <utility>


PoolAllocator::~PoolAllocator() noexcept {
	clear();
}



PoolAllocator::PoolAllocator(PoolAllocator&& allocator) noexcept :
	heap(std::exchange(allocator.heap, nullptr)),
	head(std::exchange(allocator.head, nullptr)),
	totalSize(allocator.totalSize),
	blockSize(allocator.blockSize),
	blockAlign(allocator.blockAlign) {}



PoolAllocator& PoolAllocator::operator=(PoolAllocator&& allocator) noexcept {

	heap = std::exchange(allocator.heap, nullptr);
	head = std::exchange(allocator.head, nullptr);
	totalSize = allocator.totalSize;
	blockSize = allocator.blockSize;
	blockAlign = allocator.blockAlign;

	return *this;

}



void PoolAllocator::create(AddressT blockCount, AddressT blockSize, AlignT blockAlign) {

	clear();

	if (blockCount) {

		AddressT baseSize = Math::max(blockSize, sizeof(Storage));
		AlignT baseAlign = Math::max(blockAlign, alignof(Storage));
		AddressT alignedSize = Math::alignUp(baseSize, baseAlign);
		AddressT heapSize = blockCount * alignedSize;

		this->totalSize = heapSize;
		this->blockSize = alignedSize;
		this->blockAlign = baseAlign;
		this->heap = static_cast<u8*>(::operator new(heapSize, std::align_val_t(baseAlign)));

		generatePool();

		head = reinterpret_cast<Storage*>(heap);

	#ifdef ARC_ALLOCATOR_DEBUG_LOG
		LogD("Pool Allocator").print("Pool created at %p. Block size: %d, total size: %d,", heap, alignedSize, totalSize);
	#endif

	}

}



void PoolAllocator::clear() noexcept {

	if (heap) {

		::operator delete(heap, std::align_val_t(blockAlign));

#ifdef ARC_ALLOCATOR_DEBUG_LOG
		LogD("Pool Allocator").print("Pool destroyed at %p.", heap);
#endif

		heap = nullptr;
		head = nullptr;
		totalSize = 0;
		blockSize = 0;
		blockAlign = 0;

	}

}



[[nodiscard]] void* PoolAllocator::allocate() {

	if (!head) {
		throw std::bad_alloc();
	}

	void* allocPtr = head;
	head = head->next;

#ifdef ARC_ALLOCATOR_DEBUG_LOG
	LogD("Pool Allocator").print("Pool %p allocated memory at %p.", heap, allocPtr);
#endif

	return allocPtr;

}



void PoolAllocator::deallocate(void* ptr) noexcept {

	if (ptr) {

		Storage* storagePtr = ::new(ptr) Storage(head);
		head = storagePtr;

#ifdef ARC_ALLOCATOR_DEBUG_LOG
		LogD("Pool Allocator").print("Pool %p deallocated memory at %p.", heap, ptr);
#endif

	}

#ifdef ARC_ALLOCATOR_DEBUG_LOG
	else {
		LogD("Pool Allocator").print("Pool %p received invalid pointer %p.", heap, ptr);
	}
#endif

}



void PoolAllocator::generatePool() {

	AddressT elements = totalSize / blockSize;

	for (AddressT i = 0; i < elements; i++) {

		u8* ptr = heap + i * blockSize;
		u8* next = ptr + blockSize;

		if (i == (elements - 1)) {
			next = nullptr;
		}

		::new(ptr) Storage(reinterpret_cast<Storage*>(next));

	}

}