#include "chunkallocator.h"
#include "util/math.h"
#include "util/log.h"
#include "arcconfig.h"
#include <new>



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
		AddressT alignedSize = Math::alignUp(baseSize, baseAlign);
		AddressT chunkSize = chunkBlocks * alignedSize + sizeof(ChunkLink);

		this->totalSize = chunkSize;
		this->blockSize = alignedSize;
		this->blockAlign = baseAlign;
		this->chunkBlocks = chunkBlocks;
		this->heap = static_cast<Byte*>(::operator new(chunkSize, std::align_val_t(baseAlign)));

		generateChunk(heap);

		head = reinterpret_cast<Storage*>(heap);

#ifdef ARC_ALLOCATOR_DEBUG_LOG
		Log::debug("Chunk Allocator", "Chunk created at %p. Block size: %d, chunk size: %d,", heap, alignedSize, totalSize);
#endif

	}

}



void ChunkAllocator::clear() noexcept {

	Byte* chunk = heap;

	while (chunk) {

		Byte* nextChunk = getChunkLink(chunk)->next;
		::operator delete(chunk, std::align_val_t(blockAlign));

	#ifdef ARC_ALLOCATOR_DEBUG_LOG
		Log::debug("Chunk Allocator", "Chunk destroyed at %p.", heap);
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
		Byte* chunk = static_cast<Byte*>(::operator new(chunkSize, std::align_val_t(blockAlign)));

		//Populate chunk
		generateChunk(chunk);

		//Insert chunk to the heap's front
		getChunkLink(chunk)->next = heap;
		heap = chunk;
		head = reinterpret_cast<Storage*>(chunk);

#ifdef ARC_ALLOCATOR_DEBUG_LOG
			Log::debug("Chunk Allocator", "Chunk created at %p. Block size: %d, chunk size: %d,", heap, blockSize, chunkSize);
#endif

	}

	//Acquire the head pointer and prepare to return it
	void* allocPtr = head;

	//Next head is the next block of the previous head
	head = head->next;

#ifdef ARC_ALLOCATOR_DEBUG_LOG
	Log::debug("Chunk Allocator", "Chunk %p allocated memory at %p.", heap, allocPtr);
#endif

	return allocPtr;

}



void ChunkAllocator::deallocate(void* ptr) noexcept {

	if (ptr) {

		//Simply relink it to head
		Storage* storagePtr = ::new(ptr) Storage(head);
		head = storagePtr;

#ifdef ARC_ALLOCATOR_DEBUG_LOG
		Log::debug("Pool Allocator", "Pool %p deallocated memory at %p.", heap, ptr);
#endif

	}

}



void ChunkAllocator::generateChunk(Byte* chunkPtr) {

	for (AddressT i = 0; i < chunkBlocks; i++) {

		Byte* ptr = chunkPtr + i * blockSize;
		Byte* next = ptr + blockSize;

		if (i == (chunkBlocks - 1)) {
			next = nullptr;
		}

		::new(ptr) Storage(reinterpret_cast<Storage*>(next));

	}

	::new(getChunkLink(chunkPtr)) ChunkLink(nullptr);

}



ChunkAllocator::ChunkLink* ChunkAllocator::getChunkLink(Byte* chunkPtr) const noexcept {
	return reinterpret_cast<ChunkLink*>(chunkPtr + blockSize * chunkBlocks);
}