#pragma once

#include "types.h"



using AllocSizeT = u64;

class Allocator {

public:

	constexpr static inline std::align_val_t allocatorAlignBase = std::align_val_t(64);

	constexpr Allocator() : heap(nullptr), totalSize(0) {}

	virtual void* allocate(AllocSizeT size) = 0;
	virtual void deallocate(void* ptr) = 0;
	virtual void clear() = 0;

	virtual AllocSizeT getTotalSize() = 0;
	virtual u64 getAllocatedBlocks() = 0;
	virtual bool contains(void* ptr) = 0;
	virtual AllocSizeT getSize(void* ptr) = 0;

protected:

	void* heap;
	AllocSizeT totalSize;

};