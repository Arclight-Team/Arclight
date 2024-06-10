/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ChunkAllocator.hpp
 */

#pragma once

#include "Common/Types.hpp"


/*
	ChunkAllocator

	Manages a resizeable heap divided into fixed-size blocks.
	Allows allocation/deallocation in O(1).TODO
*/
class ChunkAllocator {

public:

	//Creates a new ChunkAllocator instance. No memory is allocated upon construction.
	constexpr ChunkAllocator() noexcept : heap(nullptr), head(nullptr), totalSize(0), blockSize(0), blockAlign(0), chunkBlocks(0) {}

	//Memory is freed automatically. However, the user must ensure every destructor is called before destroying the allocator itself.
	~ChunkAllocator() noexcept;

	//Move allowed, copy disabled.
	ChunkAllocator(const ChunkAllocator& allocator) = delete;
	ChunkAllocator& operator=(const ChunkAllocator& allocator) = delete;
	ChunkAllocator(ChunkAllocator&& allocator) noexcept;
	ChunkAllocator& operator=(ChunkAllocator&& allocator) noexcept;


	/*
		Creates a new heap which is partitioned into chunks of blocks. The previously created heap will be destroyed.
		May throw std::bad_alloc if initial allocation failed.

		blockSize:		Specifies the size of the block.
		blockAlign:		Specifies the alignment of the block.
		chunkBlocks:	Number of blocks per chunk

		The actual block size/alignment has a minimum as specified by Storage.
	*/
	void create(AddressT blockSize, AlignT blockAlign, AddressT chunkBlocks);


	/*
		Creates a new heap whereas block size/alignment is deduced by T.
		See create(blockSize, blockAlign, chunkBlocks) for more information.
	*/
	template<class T>
	void create(AddressT chunkBlocks) {
		create(sizeof(T), alignof(T), chunkBlocks);
	}


	//Deallocates the heap if non-null.
	void clear() noexcept;


	/*
		Acquires a block of allocated memory. Throws std::bad_alloc if no memory has been allocated or there is no free block left.
		returns:	A pointer to the allocated block.
	*/
	[[nodiscard]] void* allocate();


	/*
		Deallocates a pointer. Deallocating memory as pointed to by the pointer which he does not own results in undefined behaviour.
		ptr:		The pointer to be deallocated. nullptr has no effect.
	*/
	void deallocate(void* ptr) noexcept;


private:

	//Stores a pointer to the next free block.
	struct Storage {

		constexpr Storage(Storage* next) noexcept : next(next) {}
		Storage* next;

	};

	//Chunk link to the next chunk.
	struct ChunkLink {

		constexpr ChunkLink(u8* next) noexcept : next(next) {}
		u8* next;

	};


	/*
		Initializes the chunk given by the pointer.
		chunkPtr:	Pointer to the chunk's start. Result is undefined if it's null.
	*/
	void generateChunk(u8* chunkPtr);


	/*
		Returns a pointer to the chunk's link.
		chunkPtr:	Pointer to the chunk's link. Result is undefined if it's null.
	*/
	ChunkLink* getChunkLink(u8* chunkPtr) const noexcept;


	u8* heap;
	Storage* head;
	AddressT totalSize;

	AddressT blockSize;
	AlignT blockAlign;
	AddressT chunkBlocks;

};