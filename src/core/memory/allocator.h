#pragma once

#include "types.h"


class Allocator {

public:

	using AllocSizeT = u64;

	virtual ~Allocator() {};

	virtual void create(AllocSizeT size) = 0;
	virtual void destroy() = 0;

	virtual void* allocate(AllocSizeT size) = 0;
	virtual void deallocate(void* ptr) = 0;
	virtual void clear() = 0;

	virtual AllocSizeT getTotalSize() = 0;
	virtual u64 getAllocatedBlocks() = 0;
	virtual bool contains(void* ptr) = 0;
	virtual AllocSizeT getSize(void* ptr) = 0;

};