#include "memory.h"

Allocator::Allocator() {}
Allocator::~Allocator() {}


ListAllocator::ListAllocator() {}
ListAllocator::ListAllocator(void* start, u64 size) {
	initialize(start, size);
}
ListAllocator::~ListAllocator() {
	destroy();
}



void* ListAllocator::allocate(u64 size) {
	MemoryBlock* pMBStart = reinterpret_cast<MemoryBlock*>(pHeapStart);
	MemoryBlock* pMBCur = reinterpret_cast<MemoryBlock*>(pHeapStart);
	MemoryBlock* pMBEnd = reinterpret_cast<MemoryBlock*>(reinterpret_cast<u8*>(pHeapStart) + nHeapSize);
	MemoryBlock* pMBPrev = nullptr;
	MemoryBlock* pMBNext = pMBEnd;

	if (!pMBCur) {
		Log::warn("Heap", "Heap is null");
		// terminate();
		return nullptr;
	}

	if (size == 0) {
		Log::debug("Heap", "Size is zero");
		return nullptr;
	}

	// align to 64 bytes
	size &= ~(sizeof(MemoryBlock) - 1);
	// memory block must be allocated inside the heap
	size += sizeof(MemoryBlock);

	for (u32 i = 0; i <= nAllocatedBlocks && pMBCur != pMBEnd; i++) {

		// memory block is null or is not valid
		if (checkMBCorruption(pMBCur))
			return nullptr;

		if (!pMBCur->used && size <= pMBCur->size) {
			pMBNext = reinterpret_cast<MemoryBlock*>(reinterpret_cast<u8*>(pMBCur) + size);
			if (pMBNext < pMBEnd) {
				if (!pMBNext->used) {
					new (pMBNext) MemoryBlock(pMBCur->size - size, 0, false, this, pMBCur, pMBEnd);
				}
			}
			else {
				pMBNext = pMBEnd;
			}

			new (pMBCur) MemoryBlock(size, 0, true, this, pMBPrev, pMBNext);
			nAllocatedBlocks++;
			return pMBCur + 1;
		}

		pMBPrev = pMBCur;
		pMBCur = pMBCur->pNext;
	}

	// not enough memory
	Log::debug("Heap", "Not enough memory");
	return nullptr;
}

void ListAllocator::deallocate(void* ptr) {
	if (!ptr) {
		Log::debug("Heap", "Tried deallocating null memory");
		return;
	}

	MemoryBlock* pMB = reinterpret_cast<MemoryBlock*>(ptr) - 1;

	if (!pMB->valid()) {
		Log::debug("Heap", "Tried deallocating unknown memory");
		return;
	}

	if (pMB->pAllocator != this) {
		Log::debug("Heap", "Tried deallocating memory not allocated by this allocator");
		return;
	}

	deallocateDirect(pMB);
}

void ListAllocator::deallocateAll() {
	initialize(pHeapStart, nHeapSize);
}

void* ListAllocator::reallocate(void* ptr, u64 newSize) {
	if (!ptr) {
		Log::debug("Heap", "Tried reallocating null memory");
		return nullptr;
	}

	MemoryBlock* pMB = reinterpret_cast<MemoryBlock*>(ptr) - 1;

	if (!pMB->valid()) {
		Log::debug("Heap", "Tried reallocating unknown memory");
		return nullptr;
	}

	if (!checkMBOwner(pMB)) {
		Log::debug("Heap", "Tried reallocating memory not allocated by this allocator");
		return nullptr;
	}

	void* pNew = allocate(newSize);
	if (!pNew) {
		Log::warn("Heap", "Couldn't allocate memory for reallocation");
		return nullptr;
	}

	std::memcpy(pNew, ptr, std::min(pMB->size - sizeof(MemoryBlock), newSize));

	deallocate(ptr);

	return pNew;
}

void ListAllocator::visitAll(VisitorFunc func, u32 param) {
	MemoryBlock* pMBCur = reinterpret_cast<MemoryBlock*>(pHeapStart);
	MemoryBlock* pMBEnd = reinterpret_cast<MemoryBlock*>(reinterpret_cast<u8*>(pHeapStart) + nHeapSize);
	MemoryBlock* pMBNext = nullptr;

	if (!pMBCur) {
		Log::warn("Heap", "Heap is null");
		// terminate();
		return;
	}

	if (!func) {
		Log::debug("Heap", "Visitor function is null");
		return;
	}

	for (u32 i = 0; i <= nAllocatedBlocks && pMBCur != pMBEnd; i++) {

		// memory block is null or is not valid
		if (checkMBCorruption(pMBCur))
			return;

		// save pointer to next memory block in case of deallocation
		pMBNext = pMBCur->pNext;

		if (pMBCur->used) {
			func(this, pMBCur, param);
		}

		pMBCur = pMBNext;
	}
}


u64 ListAllocator::getTotalSize() {
	return nHeapSize - sizeof(MemoryBlock);
}

u32 ListAllocator::getAllocatedBlocks() {
	return nAllocatedBlocks;
}

bool ListAllocator::contains(void* ptr) {
	return (pHeapStart >= ptr) && (ptr < (reinterpret_cast<u8*>(pHeapStart) + nHeapSize));
}

u64 ListAllocator::getSize(void* ptr) {
	if (!ptr) {
		Log::debug("Heap", "Tried getting size of null memory");
		return 0;
	}

	MemoryBlock* pMB = reinterpret_cast<MemoryBlock*>(ptr) - 1;

	if (!pMB->valid()) {
		Log::debug("Heap", "Tried getting size of unknown memory");
		return 0;
	}

	// should this be ignored?
	if (!checkMBOwner(pMB)) {
		Log::debug("Heap", "Tried getting size of memory not allocated by this allocator");
		return 0;
	}

	return pMB->size - sizeof(MemoryBlock);
}



void ListAllocator::initialize(void* start, u64 size) {
	// align to 64 bytes
	nHeapSize = size & ~(sizeof(MemoryBlock) - 1);
	pHeapStart = start;
	nAllocatedBlocks = 0;

	std::memset(pHeapStart, 0, nHeapSize);

	MemoryBlock* pMBStart = reinterpret_cast<MemoryBlock*>(pHeapStart);
	MemoryBlock* pMBEnd = reinterpret_cast<MemoryBlock*>(reinterpret_cast<u8*>(pHeapStart) + nHeapSize);
	new (pMBStart) MemoryBlock(nHeapSize, 0, false, this, nullptr, pMBEnd);
}

void ListAllocator::destroy() {
	std::memset(pHeapStart, 0, nHeapSize);
}

#if _DEBUG
void ListAllocator::dumpMemory() {
	visitAll(&ListAllocator::dumpMemoryVisitor, 0);
}
#endif

void ListAllocator::deallocateTempMemory() {
	visitAll(&ListAllocator::deallocateTempVisitor, 1);
}



u64 ListAllocator::getMBlockSize(void* ptr) {
	if (!ptr) {
		Log::debug("Heap", "Tried getting size of null memory");
		return 0;
	}

	MemoryBlock* pMB = reinterpret_cast<MemoryBlock*>(ptr) - 1;

	if (!pMB->valid()) {
		Log::debug("Heap", "Tried getting size of unknown memory");
		return 0;
	}

	return pMB->size - sizeof(MemoryBlock);
}

u32 ListAllocator::getMBlockID(void* ptr) {
	if (!ptr) {
		Log::debug("Heap", "Tried getting memory block ID of null memory");
		return 0;
	}

	MemoryBlock* pMB = reinterpret_cast<MemoryBlock*>(ptr) - 1;

	if (!pMB->valid()) {
		Log::debug("Heap", "Tried getting memory block ID of unknown memory");
		return 0;
	}

	return pMB->id;
}

void ListAllocator::setMBlockID(void* ptr, u32 id) {
	if (!ptr) {
		Log::debug("Heap", "Tried setting memory block ID of null memory");
		return;
	}

	MemoryBlock* pMB = reinterpret_cast<MemoryBlock*>(ptr) - 1;

	if (!pMB->valid()) {
		Log::debug("Heap", "Tried setting memory block ID of unknown memory");
		return;
	}

	pMB->id = id;
}


void ListAllocator::ListAllocator::deallocateDirect(MemoryBlock* mBlock) {
	if (contains(mBlock)) {
		mBlock->used = false;

		// unlink memory block
		if (mBlock->pNext) mBlock->pNext->pPrev = mBlock->pPrev;
		if (mBlock->pPrev) mBlock->pPrev->pNext = mBlock->pNext;

		nAllocatedBlocks--;
	}
}
#include <typeinfo>
#if _DEBUG
void ListAllocator::dumpMemoryVisitor(Allocator* allocator, void* mBlock, u32 param) {
	ListAllocator* pAllocator = reinterpret_cast<ListAllocator*>(allocator);
	MemoryBlock* pMB = reinterpret_cast<MemoryBlock*>(mBlock);

	Log::debug("Heap", "Memory Block information:");
	Log::debug("Heap", "    Address:    0x%X", pMB);
	Log::debug("Heap", "    Tag:        0x%X", pMB->tag);
	//Log::debug("Heap", "    ID:         0x%X", pMB->id);
	Log::debug("Heap", "    Used:       %s", pMB->used ? "true" : "false");
	Log::debug("Heap", "    Size:       0x%X (total: 0x%X)", pMB->size - sizeof(MemoryBlock), pMB->size);
	Log::debug("Heap", "    Allocator:  0x%X", pMB->pAllocator);
	Log::debug("Heap", "    Previous:   0x%X", pMB->pPrev);
	Log::debug("Heap", "    Next:       0x%X", pMB->pNext);
}
#endif

void ListAllocator::deallocateTempVisitor(Allocator* allocator, void* mBlock, u32 param) {
	ListAllocator* pAllocator = reinterpret_cast<ListAllocator*>(allocator);
	MemoryBlock* pMB = reinterpret_cast<MemoryBlock*>(mBlock);

	if (pMB->id == param) {
		pAllocator->deallocateDirect(pMB);
	}
}