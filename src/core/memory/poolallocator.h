#pragma once

#include <new>
#include "util/assert.h"
#include "util/math.h"
#include "util/log.h"
#include "types.h"
#include "arcbuild.h"
#include "arcconfig.h"



template<class T, AlignT Align = sizeof(SystemT) * 8>
class PoolAllocator {

	struct Storage {

		constexpr Storage(Storage* next) noexcept : next(next) {}

		Storage* next;

	};

public:

	constexpr static inline AddressT baseSize = Math::max(sizeof(T), sizeof(Storage));
	constexpr static inline AlignT baseAlignment = Math::max(alignof(T), alignof(Storage));
	constexpr static inline AddressT alignedSize = Math::alignUp(baseSize, baseAlignment);



	constexpr PoolAllocator() noexcept : heap(nullptr), head(nullptr), totalSize(0) {}

	~PoolAllocator() noexcept {
		clear();
	}

	PoolAllocator(const PoolAllocator& allocator) = delete;
	PoolAllocator& operator=(const PoolAllocator& allocator) = delete;

	constexpr PoolAllocator(PoolAllocator&& allocator) noexcept : 
		heap(std::exchange(allocator.heap, nullptr)), 
		totalSize(std::exchange(allocator.totalSize, 0)) {}

	constexpr PoolAllocator& operator=(PoolAllocator&& allocator) noexcept {

		heap = std::exchange(allocator.heap, nullptr);
		totalSize = std::exchange(allocator.totalSize, nullptr);

		return *this;

	}



	void create(AddressT size) {

		#ifdef ARC_ALLOCATOR_DEBUG_CHECKS
			arc_assert(size % alignedSize == 0, "Requested pool size %d is not multiple of element size %d", size, alignedSize);
		#endif
			
		size = size - (size % alignedSize);

		if (!heap && size) {

			heap = static_cast<Byte*>(::operator new(size, std::align_val_t(Align)));
			totalSize = size;
			
			generatePool();

			#ifdef ARC_ALLOCATOR_DEBUG_LOG
				Log::debug("Pool Allocator", "Pool created at %p. Element size: %d, total size: %d,", heap, alignedSize, totalSize);
			#endif

		}

	}



	void clear() noexcept {

		if (heap) {

			::operator delete(heap, std::align_val_t(Align));

			#ifdef ARC_ALLOCATOR_DEBUG_LOG
				Log::debug("Pool Allocator", "Pool destroyed at %p.", heap);
			#endif

			heap = nullptr;
			head = nullptr;
			totalSize = 0;

		}

	}



	constexpr [[nodiscard]] void* allocate() {

		if (!head) {
			throw std::bad_alloc();
		}

		void* allocPtr = head;
		head = head->next;

		#ifdef ARC_ALLOCATOR_DEBUG_LOG
			Log::debug("Pool Allocator", "Pool %p allocated memory at %p.", heap, allocPtr);
		#endif

		return allocPtr;

	}



	constexpr void deallocate(void* ptr) noexcept {

		if (contains(ptr)) {

			::new(ptr) Storage(head);
			head = static_cast<Storage*>(ptr);

			#ifdef ARC_ALLOCATOR_DEBUG_LOG
				Log::debug("Pool Allocator", "Pool %p deallocated memory at %p.", heap, ptr);
			#endif

		}

		#ifdef ARC_ALLOCATOR_DEBUG_LOG
			else {
				Log::debug("Pool Allocator", "Pool %p received invalid pointer %p.", heap, ptr);
			}
		#endif

	}



	template<class... Args>
	T* construct(void* object, Args&&... args) noexcept(noexcept(T(std::forward<Args>(args)...))) {
		return ::new(object) T(std::forward<Args>(args)...);
	}



	void destroy(T* object) noexcept(noexcept(std::declval<T>().~T())) {
		object->~T();
	}



private:

	constexpr AddressT allocatableElements() const noexcept {
		return totalSize / alignedSize;
	}



	constexpr bool contains(void* ptr) const noexcept {
		return ptr >= heap && ptr < (heap + totalSize) && Math::isAligned(Math::address(ptr), baseAlignment);
	}



	void generatePool() {

		AddressT elements = allocatableElements();

		for (AddressT i = 0; i < elements; i++) {

			Byte* ptr = heap + i * alignedSize;
			Byte* next = ptr + alignedSize;
			
			if (i == elements) [[unlikely]] {
				next = nullptr;
			}

			::new(ptr) Storage(reinterpret_cast<Storage*>(next));

		}

		head = reinterpret_cast<Storage*>(heap);

	}

	Byte* heap;
	Storage* head;
	AddressT totalSize;

};