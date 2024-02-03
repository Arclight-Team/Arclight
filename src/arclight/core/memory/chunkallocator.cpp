/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 chunkallocator.cpp
 */

#include "chunkallocator.hpp"
#include "math/math.hpp"
#include "util/log.hpp"
#include "arcconfig.hpp"

#include <new>
#include <utility>



ChunkAllocator::~ChunkAllocator() noexcept {
	clear();
}



ChunkAllocator::ChunkAllocator(ChunkAllocator&& allocator) noexcept :
	heap(std::exchange(allocator.heap, nullptr)),
	head(std::exchange(allocator.head, nullptr)),
	totalSize(allocator.totalSize),
	blockSize(allocator.blockSize),
	blockAlign(allocator.blockAlign),
	chunkBlocks(allocator.chunkBlocks) {}



ChunkAllocator& ChunkAllocator::operator=(ChunkAllocator&& allocator) noexcept {

	heap = std::exchange(allocator.heap, nullptr);
	head = std::exchange(allocator.head, nullptr);
	totalSize = allocator.totalSize;
	blockSize = allocator.blockSize;
	blockAlign = allocator.blockAlign;
	chunkBlocks = allocator.chunkBlocks;

	return *this;
}



void ChunkAllocator::create(AddressT blockSize, AlignT blockAlign, AddressT chunkBlocks) {

	clear();

	if (blockSize && chunkBlocks) {

		AddressT baseSize = Math::max(blockSize, sizeof(Storage));
		AlignT baseAlign = Math::max(blockAlign, alignof(Storage));
		AddressT alignedSize = Bits::alignUp(baseSize, baseAlign);
		AddressT chunkSize = chunkBlocks * alignedSize + sizeof(ChunkLink);

		this->totalSize = chunkSize;
		this->blockSize = alignedSize;
		this->blockAlign = baseAlign;
		this->chunkBlocks = chunkBlocks;
		this->heap = static_cast<u8*>(::operator new(chunkSize, std::align_val_t(baseAlign)));

		generateChunk(heap);

		head = reinterpret_cast<Storage*>(heap);

#ifdef ARC_ALLOCATOR_DEBUG_LOG
		LogD("Chunk Allocator").print("Chunk created at %p. Block size: %d, chunk size: %d,", heap, alignedSize, totalSize);
#endif

	}

}



void ChunkAllocator::clear() noexcept {

	u8* chunk = heap;

	while (chunk) {

		u8* nextChunk = getChunkLink(chunk)->next;
		::operator delete(chunk, std::align_val_t(blockAlign));

	#ifdef ARC_ALLOCATOR_DEBUG_LOG
		LogD("Chunk Allocator").print("Chunk destroyed at %p.", heap);
	#endif

		chunk = nextChunk;

	}

	heap = nullptr;
	head = nullptr;
	totalSize = 0;
	blockSize = 0;
	blockAlign = 0;
	chunkBlocks = 0;

}



[[nodiscard]] void* ChunkAllocator::allocate() {

	if (!head) {

		//If all chunks are full, allocate a new one
		AddressT chunkSize = blockSize * chunkBlocks + sizeof(ChunkLink);
		u8* chunk = static_cast<u8*>(::operator new(chunkSize, std::align_val_t(blockAlign)));

		//Populate chunk
		generateChunk(chunk);

		//Insert chunk to the heap's front
		getChunkLink(chunk)->next = heap;
		heap = chunk;
		head = reinterpret_cast<Storage*>(chunk);

#ifdef ARC_ALLOCATOR_DEBUG_LOG
			LogD("Chunk Allocator").print("Chunk created at %p. Block size: %d, chunk size: %d,", heap, blockSize, chunkSize);
#endif

	}

	//Acquire the head pointer and prepare to return it
	void* allocPtr = head;

	//Next head is the next block of the previous head
	head = head->next;

#ifdef ARC_ALLOCATOR_DEBUG_LOG
	LogD("Chunk Allocator").print("Chunk %p allocated memory at %p.", heap, allocPtr);
#endif

	return allocPtr;

}



void ChunkAllocator::deallocate(void* ptr) noexcept {

	if (ptr) {

		//Simply relink it to head
		Storage* storagePtr = ::new(ptr) Storage(head);
		head = storagePtr;

#ifdef ARC_ALLOCATOR_DEBUG_LOG
		LogD("Chunk Allocator").print("Chunk %p deallocated memory at %p.", heap, ptr);
#endif

	}

}



void ChunkAllocator::generateChunk(u8* chunkPtr) {

	for (AddressT i = 0; i < chunkBlocks; i++) {

		u8* ptr = chunkPtr + i * blockSize;
		u8* next = ptr + blockSize;

		if (i == (chunkBlocks - 1)) {
			next = nullptr;
		}

		::new(ptr) Storage(reinterpret_cast<Storage*>(next));

	}

	::new(getChunkLink(chunkPtr)) ChunkLink(nullptr);

}



ChunkAllocator::ChunkLink* ChunkAllocator::getChunkLink(u8* chunkPtr) const noexcept {
	return reinterpret_cast<ChunkLink*>(chunkPtr + blockSize * chunkBlocks);
}