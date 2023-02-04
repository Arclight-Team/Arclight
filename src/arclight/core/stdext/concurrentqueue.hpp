/*

	Original work: Copyright (c) 2020 Erik Rigtorp <erik@rigtorp.se>
	Modified work: Copyright (c) 2021 Arclight Team

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

*/

/*
	Concurrent Queue for multiple consumers/producers with atomic operations.
	This code has been optimized to relax atomic reordering.
	All functions except constructor and destructor guarantee no-throw behaviour with the given constraints of T.
*/

#pragma once

#include "types.hpp"

#include <atomic>
#include <cstddef>
#include <new>


template<class T, u32 Size>
class ConcurrentQueue final {

	constexpr static inline std::size_t hdiSize = std::hardware_destructive_interference_size;

	struct alignas(hdiSize) Storage {

		~Storage() {

			if (index & (activeBit << activeShift)) {
				reinterpret_cast<T*>(data)->~T();
			}

		}

		void acquire(T&& object) noexcept {
			::new(data) T(std::move(object));
		}

		void release(T& object) noexcept {

			T& old = *reinterpret_cast<T*>(data);
			object = std::move(old);
			old.~T();

		}

		u64 getIndex() const noexcept {
			return index.load(std::memory_order_acquire);
		}

		void setIndex(u64 i) noexcept {
			index.store(i, std::memory_order_release);
		}

		constexpr static inline u64 activeBit = 1;
		constexpr static inline u8	activeShift = 0;
		constexpr static inline u8	indexShift = 1;

	private:

		std::atomic<u64> index;
		alignas(T) u8 data[sizeof(T)];

	};

public:

	static_assert(std::is_nothrow_move_constructible_v<T>, "ConcurrentQueue requires T to be nothrow move-constructible");
	static_assert(std::is_nothrow_destructible_v<T>, "ConcurrentQueue requires T to be nothrow destructible");

	constexpr ConcurrentQueue() {

		storage = new Storage[Size];
		head.store(0, std::memory_order_relaxed);
		tail.store(0, std::memory_order_relaxed);
		std::atomic_thread_fence(std::memory_order_release);

	}

	~ConcurrentQueue() {

		if (storage) {

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
			if ((currentTail / Size) << Storage::indexShift == s.getIndex()) {
				
				//Try incrementing the tail now. Fails if it has been modified by a different thread, retry in that case.
				if (tail.compare_exchange_strong(currentTail, currentTail + 1, std::memory_order_acq_rel, std::memory_order_acquire)) {

					s.acquire(std::move(element));
					s.setIndex(((currentTail / Size) << Storage::indexShift) | (Storage::activeBit << Storage::activeShift));
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



	bool pop(T& element) noexcept {
	
		u64 currentHead = head.load(std::memory_order_acquire);

		while (true) {

			Storage& s = storage[currentHead % Size];

			//When the index holds the correct state, attempt to push later
			if ((((currentHead / Size) << Storage::indexShift) | (Storage::activeBit << Storage::activeShift)) == s.getIndex()) {

				//Try incrementing the head now. Fails if it has been modified by a different thread, retry in that case.
				if (head.compare_exchange_strong(currentHead, currentHead + 1, std::memory_order_acq_rel, std::memory_order_acquire)) {

					s.release(element);
					s.setIndex((currentHead / Size + 1) << Storage::indexShift);
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
		u64 tailIndex = tail.load(std::memory_order_relaxed);

		return tailIndex - headIndex;

	}



	//Returns whether the queue is empty. It's NOT a qualitative measurement since it might not represent the actual current state!
	bool empty() const noexcept {

		std::atomic_thread_fence(std::memory_order_acquire);
		u64 headIndex = head.load(std::memory_order_relaxed);
		u64 tailIndex = tail.load(std::memory_order_relaxed);

		return headIndex == tailIndex;

	}

private:

	Storage* storage;
	alignas(hdiSize) std::atomic<u64> head;
	alignas(hdiSize) std::atomic<u64> tail;

};