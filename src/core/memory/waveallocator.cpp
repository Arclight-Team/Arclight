#include "waveallocator.h"
#include "util/math.h"
#include "util/log.h"
#include "arcconfig.h"
#include <new>
#include <algorithm>



WaveAllocator::~WaveAllocator() noexcept {
	clear();
}



WaveAllocator::WaveAllocator(WaveAllocator&& allocator) noexcept :
	heap(std::exchange(allocator.heap, nullptr)),
	head(std::exchange(allocator.head, nullptr)),
	totalSize(allocator.totalSize),
	blockSize(allocator.blockSize),
	blockAlign(allocator.blockAlign),
	waveBlocks(allocator.waveBlocks) {}



WaveAllocator& WaveAllocator::operator=(WaveAllocator&& allocator) noexcept {

	heap = std::exchange(allocator.heap, nullptr);
	head = std::exchange(allocator.head, nullptr);
	totalSize = allocator.totalSize;
	blockSize = allocator.blockSize;
	blockAlign = allocator.blockAlign;
	waveBlocks = allocator.waveBlocks;

	return *this;
}



void WaveAllocator::create(AddressT blockSize, AlignT blockAlign, AddressT waveBlocks) {

	clear();

	if (blockSize && waveBlocks) {

		AddressT baseSize = Math::max(blockSize, sizeof(Storage));
		AlignT baseAlign = Math::max(blockAlign, alignof(Storage));
		AddressT alignedSize = Math::alignUp(baseSize, baseAlign);
		AddressT waveSize = waveBlocks * alignedSize + sizeof(WaveLink);

		this->heap = static_cast<Byte*>(::operator new(waveSize, std::align_val_t(baseAlign)));
		this->totalSize = waveSize;
		this->blockSize = alignedSize;
		this->blockAlign = baseAlign;
		this->waveBlocks = waveBlocks;

		generateWave(heap);

		head = reinterpret_cast<Storage*>(heap);

#ifdef ARC_ALLOCATOR_DEBUG_LOG
		Log::debug("Wave Allocator", "Wave created at %p. Block size: %d, wave size: %d,", heap, alignedSize, totalSize);
#endif

	}

}



void WaveAllocator::clear() noexcept {

	Byte* wave = heap;

	while (wave) {

		Byte* nextWave = getWaveLink(wave)->next;
		::operator delete(wave, std::align_val_t(blockAlign));

	#ifdef ARC_ALLOCATOR_DEBUG_LOG
		Log::debug("Wave Allocator", "Wave destroyed at %p.", heap);
	#endif

		wave = nextWave;

	}

	heap = nullptr;
	head = nullptr;
	totalSize = 0;
	blockSize = 0;
	blockAlign = 0;
	waveBlocks = 0;

}



[[nodiscard]] void* WaveAllocator::allocate() {

	if (!head) {

		//If all waves are full, allocate a new one
		AddressT waveSize = blockSize * waveBlocks + sizeof(WaveLink);
		Byte* wave = static_cast<Byte*>(::operator new(waveSize, std::align_val_t(blockAlign)));

		//Populate wave
		generateWave(wave);

		//Insert wave to the heap's front
		getWaveLink(wave)->next = heap;
		heap = wave;
		head = reinterpret_cast<Storage*>(wave);

#ifdef ARC_ALLOCATOR_DEBUG_LOG
			Log::debug("Wave Allocator", "Wave created at %p. Block size: %d, wave size: %d,", heap, blockSize, waveSize);
#endif

	}

	//Acquire the head pointer and prepare to return it
	void* allocPtr = head;

	//Next head is the next block of the previous head
	head = head->next;

#ifdef ARC_ALLOCATOR_DEBUG_LOG
	Log::debug("Wave Allocator", "Wave %p allocated memory at %p.", heap, allocPtr);
#endif

	return allocPtr;

}



void WaveAllocator::deallocate(void* ptr) noexcept {

	if (ptr) {

		//Simply relink it to head
		Storage* storagePtr = ::new(ptr) Storage(head);
		head = storagePtr;

#ifdef ARC_ALLOCATOR_DEBUG_LOG
		Log::debug("Pool Allocator", "Pool %p deallocated memory at %p.", heap, ptr);
#endif

	}

}



void WaveAllocator::generateWave(Byte* wavePtr) {

	for (AddressT i = 0; i < waveBlocks; i++) {

		Byte* ptr = wavePtr + i * blockSize;
		Byte* next = ptr + blockSize;

		if (i == (waveBlocks - 1)) {
			next = nullptr;
		}

		::new(ptr) Storage(reinterpret_cast<Storage*>(next));

	}

	::new(getWaveLink(wavePtr)) WaveLink(nullptr);

}



WaveAllocator::WaveLink* WaveAllocator::getWaveLink(Byte* wavePtr) const noexcept {
	return reinterpret_cast<WaveLink*>(wavePtr + blockSize * waveBlocks);
}