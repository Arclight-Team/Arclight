#pragma once
#include "types.h"
#include "util/log.h"
#include <cstring>		// for std::memset, std::memcpy
#include <algorithm>	// for std::min

class Allocator {
public:

	typedef void (*VisitorFunc)(Allocator*, void*, u32);

	Allocator();
	virtual ~Allocator();

	virtual void* allocate(u64 size) = 0;
	virtual void deallocate(void* ptr) = 0;
	virtual void deallocateAll() = 0;
	virtual void* reallocate(void* ptr, u64 newSize) = 0;
	virtual void visitAll(VisitorFunc func, u32 param) = 0;

	virtual u64 getTotalSize() = 0;
	virtual u32 getAllocatedBlocks() = 0;
	virtual bool contains(void* ptr) = 0;
	virtual u64 getSize(void* ptr) = 0;

};

class ListAllocator : public Allocator
{
	void* pHeapStart;
	u64 nHeapSize;
	u32 nAllocatedBlocks;

public:

	struct MemoryBlock {
		alignas(64)

		u32 tag;
		u32 id : 31;
		bool used : 1;
		u64 size;

		Allocator* pAllocator;
		MemoryBlock* pPrev;
		MemoryBlock* pNext;

		enum {
			MBTag = 0x4154534C,
		};

		inline MemoryBlock(u64 size, u32 id, bool used, Allocator* allocator, MemoryBlock* prev, MemoryBlock* next) :
			tag(MBTag), size(size), id(id), used(used), pAllocator(allocator), pPrev(prev), pNext(next) {}

		inline bool valid() {
			return tag == MBTag;
		}
	};

	ListAllocator();
	ListAllocator(void* start, u64 size);
	virtual ~ListAllocator();



	virtual void* allocate(u64 size) override;

	virtual void deallocate(void* ptr) override;

	virtual void deallocateAll() override;

	virtual void* reallocate(void* ptr, u64 newSize) override;

	virtual void visitAll(VisitorFunc func, u32 param) override;


	virtual u64 getTotalSize() override;

	virtual u32 getAllocatedBlocks() override;

	virtual bool contains(void* ptr) override;

	virtual u64 getSize(void* ptr) override;




	void initialize(void* start, u64 size);

	void destroy();

#if _DEBUG
	void dumpMemory();
#endif

	void deallocateTempMemory();



	static inline u64 alignment() {
		return sizeof(MemoryBlock);
	}

	static u64 getMBlockSize(void* ptr);

	static u32 getMBlockID(void* ptr);

	static void setMBlockID(void* ptr, u32 id);


private:

	inline bool checkMBOwner(MemoryBlock* mBlock) {
		return mBlock->pAllocator == this;
	}

	inline bool checkMBCorruption(MemoryBlock* mBlock) {
		if (!mBlock || !mBlock->valid() || mBlock->pAllocator != this) {
			Log::warn("Heap", "Memory block corruption detected");
			// terminate();
			return true;
		}
		return false;
	}

	void deallocateDirect(MemoryBlock* mBlock);

#if _DEBUG
	static void dumpMemoryVisitor(Allocator* allocator, void* mBlock, u32 param);
#endif

	static void deallocateTempVisitor(Allocator* allocator, void* mBlock, u32 param);

};