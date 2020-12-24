#pragma once


class Allocator {

public:

	Allocator();
	virtual ~Allocator();


	virtual void* allocate(u64 size) = 0;
	virtual void deallocate(void* ptr) = 0;
	virtual void* reallocate(void* ptr, u64 newSize) = 0;
	virtual void clear() = 0;

	virtual u64 getTotalSize() = 0;
	virtual u32 getAllocatedBlocks() = 0;
	virtual bool contains(void* ptr) = 0;
	virtual u64 getSize(void* ptr) = 0;

};