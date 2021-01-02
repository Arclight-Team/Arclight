#pragma once

#include <atomic>
#include "types.h"



template<class T, u32 Size>
class ConcurrentQueue final {

	constexpr static inline std::size_t hdiSize = std::hardware_destructive_interference_size;

	struct alignas(hdiSize) Storage {

		Storage() {
			index.store(0, std::memory_order_relaxed);
		}

		~Storage() {

			if (index & (activeBit << activeShift)) {
				reinterpret_cast<T*>(&data)->~T();
			}

		}

		constexpr static inline u64 activeBit = 1;
		constexpr static inline u8	activeShift = 0;
		constexpr static inline u8	indexShift = 1;

		std::atomic<u64> index;
		std::aligned_storage_t<sizeof(T), alignof(T)> data;

	};

public:

	static_assert(std::is_nothrow_move_constructible_v<T>, "ConcurrentQueue requires T to be no-throw move-constructible");

	constexpr ConcurrentQueue() {

		storage = new Storage[Size];
		head.store(0, std::memory_order_relaxed);
		tail.store(0, std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_release);

	}

	~ConcurrentQueue() {

		if (storage) {

			std::atomic_thread_fence(std::memory_order_acquire);
			u64 headIndex = head.load(std::memory_order_relaxed);
			u64 tailIndex = tail.load(std::memory_order_relaxed);

			//Finally destroy the storage
			delete[] storage;
			storage = nullptr;

		}

	}

	ConcurrentQueue(const ConcurrentQueue& queue) = delete;
	ConcurrentQueue& operator=(const ConcurrentQueue& queue) = delete;

	constexpr ConcurrentQueue(ConcurrentQueue&& queue) noexcept {

		std::atomic_thread_fence(std::memory_order_acquire);
		this->storage = std::exchange(queue.storage, nullptr);
		this->head.store(queue.head.load(std::memory_order_relaxed), std::memory_order_relaxed);
		this->tail.store(queue.tail.load(std::memory_order_relaxed), std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_release);

	}


	constexpr ConcurrentQueue& operator=(ConcurrentQueue&& queue) noexcept {
	
		std::atomic_thread_fence(std::memory_order_acquire);
		this->storage = std::exchange(queue.storage, nullptr);
		this->head.store(queue.head.load(std::memory_order_relaxed), std::memory_order_relaxed);
		this->tail.store(queue.tail.load(std::memory_order_relaxed), std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_release);

	}

	bool push(T&& element) noexcept {

		u64 currentTail = tail.load(std::memory_order_acquire);

		while (true) {

			Storage& s = storage[currentTail % Size];

			//When the index holds the correct state, attempt to push later
			if ((currentTail / Size) << Storage::indexShift == s.index.load(std::memory_order_acquire)) {
				
				//Try incrementing the tail now. Fails if it has been modified by a different thread, retry in that case.
				if (tail.compare_exchange_strong(currentTail, currentTail + 1, std::memory_order_acq_rel, std::memory_order_acquire)) {

					::new(&s.data) T(std::move(element));
					s.index.store(((currentTail / Size) << Storage::indexShift) | (Storage::activeBit << Storage::activeShift), std::memory_order_release);
					return true;

				}

			} else {

				//Wrong state, return if tail hasn't changed (meaning the queue is full)
				u64 oldTail = currentTail;
				currentTail = tail.load(std::memory_order_acquire);

				if (oldTail == currentTail) {
					break;
				}

			}

		}

		return false;

	}



	bool pop(T& element) {
	
		u64 currentHead = head.load(std::memory_order_acquire);

		while (true) {

			Storage& s = storage[currentHead % Size];

			//When the index holds the correct state, attempt to push later
			if (((currentHead / Size) << Storage::indexShift) | (Storage::activeBit << Storage::activeShift) == s.index.load(std::memory_order_acquire)) {

				//Try incrementing the head now. Fails if it has been modified by a different thread, retry in that case.
				if (head.compare_exchange_strong(currentHead, currentHead + 1, std::memory_order_acq_rel, std::memory_order_acquire)) {

					::new(&s.data) T(std::move(element));
					s.index.store((currentHead / Size + 1) << Storage::indexShift, std::memory_order_release);
					return true;

				}

			} else {

				//Wrong state, return if head hasn't changed (meaning the queue is empty)
				u64 oldHead = currentHead;
				currentHead = head.load(std::memory_order_acquire);

				if (oldHead == currentHead) {
					break;
				}

			}

		}

		return false;

	}



	//Returns the total capacity
	u32 capacity() const noexcept {
		return Size;
	}



	//Returns the size of the queue. It's NOT a qualitative measurement since it might not represent the actual current state!
	bool size() const noexcept {

		std::atomic_thread_fence(std::memory_order_acquire);
		u64 headIndex = head.load(std::memory_order_relaxed);
		u64 tailIndex = head.load(std::memory_order_relaxed);

		return tailIndex - headIndex;

	}



	//Returns whether the queue is empty. It's NOT a qualitative measurement since it might not represent the actual current state!
	bool empty() const noexcept {

		std::atomic_thread_fence(std::memory_order_acquire);
		u64 headIndex = head.load(std::memory_order_relaxed);
		u64 tailIndex = head.load(std::memory_order_relaxed);

		return headIndex == tailIndex;

	}

private:

	Storage* storage;
	alignas(hdiSize) std::atomic<u64> head;
	alignas(hdiSize) std::atomic<u64> tail;

};