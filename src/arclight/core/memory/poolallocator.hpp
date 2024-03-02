/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 poolallocator.hpp
 */

#pragma once

#include "common/types.hpp"


/*
 
	PoolAllocator
	Manages a heap divided into fixed-sized blocks.
	Allows allocation/deallocation in O(1).

	This allocator simply allocates a block of memory with create() with a specified size and count.
	The storage is immutable, meaning it cannot be reallocated without destroying the whole heap.
	Every block has a minimum size of AddressT's size (which depends on the machine) and a minimum alignment of the same.
	Typically, both evaluate to 4 bytes on 32 bit systems and 8 bytes on 64 bit systems.

	Therefore, the layout after creation is as follows:

	Head -> 0

	+-----------------------------+ 0
	|			->    1			  |
	+-----------------------------+ 1
	|			->    2			  |
	+-----------------------------+ 2
	|			->    3			  |
	+-----------------------------+ 3
	|			-> null			  |
	+-----------------------------+

	Each storage is defined as either the requested block size or a pointer to the next free block if the block itself is free.

*/
class PoolAllocator {

public:

	//Creates a new PoolAllocator instance. No memory is allocated upon construction. 
	constexpr PoolAllocator() noexcept : heap(nullptr), head(nullptr), totalSize(0), blockSize(0), blockAlign(0) {}

	//Memory is freed automatically. However, the user must ensure every destructor is called before destroying the allocator itself.
	~PoolAllocator() noexcept;

	//Move allowed, copy disabled.
	PoolAllocator(const PoolAllocator& allocator) = delete;
	PoolAllocator& operator=(const PoolAllocator& allocator) = delete;
	PoolAllocator(PoolAllocator&& allocator) noexcept;
	PoolAllocator& operator=(PoolAllocator&& allocator) noexcept;


	/*
		Creates a new heap which is partitioned into blocks. The previously created heap will be destroyed.
		May throw std::bad_alloc if initial allocation failed.

		blockCount:	Number of blocks the heap will contain.
		blockSize:	Specifies the size of the block.
		blockAlign: Specifies the alignment of the block.

		The actual block size/alignment has a minimum as specified by Storage.
	*/
	void create(AddressT blockCount, AddressT blockSize, AlignT blockAlign);


	/*
		Creates a new heap whereas block size/alignment is deduced by T.
		See create(blockCount, blockSize, blockAlign) for more information.
	*/
	template<class T>
	void create(AddressT blockCount) {
		create(blockCount, sizeof(T), alignof(T));
	}


	//Deallocates the heap if it has been created.
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

	//Initializes the internal linked list.
	void generatePool();


	//Class that stores a pointer to the next free block.
	struct Storage {

		constexpr Storage(Storage* next) noexcept : next(next) {}

		Storage* next;

	};


	u8* heap;
	Storage* head;
	AddressT totalSize;

	AddressT blockSize;
	AlignT blockAlign;

};