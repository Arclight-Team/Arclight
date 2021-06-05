#pragma once

#include "types.h"



/*
	WaveAllocator

	Manages a resizeable heap divided into fixed-size blocks.
	Allows allocation/deallocation in O(1).TODO
*/
class WaveAllocator {

public:

	//Creates a new WaveAllocator instance. No memory is allocated upon construction.
	constexpr WaveAllocator() noexcept : heap(nullptr), head(nullptr), totalSize(0), blockSize(0), blockAlign(0), waveBlocks(0) {}

	//Memory is freed automatically. However, the user must ensure every destructor is called before destroying the allocator itself.
	~WaveAllocator() noexcept;

	//Move allowed, copy disabled.
	WaveAllocator(const WaveAllocator& allocator) = delete;
	WaveAllocator& operator=(const WaveAllocator& allocator) = delete;
	WaveAllocator(WaveAllocator&& allocator) noexcept;
	WaveAllocator& operator=(WaveAllocator&& allocator) noexcept;


	/*
		Creates a new heap which is partitioned into waves of blocks. The previously created heap will be destroyed.
		May throw std::bad_alloc if initial allocation failed.

		blockSize:		Specifies the size of the block.
		blockAlign:		Specifies the alignment of the block.
		waveBlocks:		Number of blocks per wave

		The actual block size/alignment has a minimum as specified by Storage.
	*/
	void create(AddressT blockSize, AlignT blockAlign, AddressT waveBlocks);


	/*
		Creates a new heap whereas block size/alignment is deduced by T.
		See create(blockSize, blockAlign, waveBlocks) for more information.
	*/
	template<class T>
	void create(AddressT waveBlocks) {
		create(sizeof(T), alignof(T), waveBlocks);
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

	//Wave link to the next wave.
	struct WaveLink {

		constexpr WaveLink(Byte* next) noexcept : next(next) {}
		Byte* next;

	};


	/*
		Initializes the wave given by the pointer.
		wavePtr:	Pointer to the wave's start. Result is undefined if it's null.
	*/
	void generateWave(Byte* wavePtr);


	/*
		Returns a pointer to the wave's link.
		wavePtr:	Pointer to the wave's link. Result is undefined if it's null.
	*/
	WaveLink* getWaveLink(Byte* wavePtr) const noexcept;


	Byte* heap;
	Storage* head;
	AddressT totalSize;

	AddressT blockSize;
	AlignT blockAlign;
	AddressT waveBlocks;

};