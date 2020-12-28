#pragma once

#include <atomic>
#include "types.h"


struct StandardResizePolicy {

	static constexpr u32 increment(u32 base) {
		return base / 2;
	}

};


template<class T, class ResizePolicy = StandardResizePolicy>
class ConcurrentQueue final {

public:

	static_assert(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>, "ConcurrentQueue requires T to be move-constructible and move-assignable");

	constexpr ConcurrentQueue() {}
	~ConcurrentQueue() {}

	ConcurrentQueue(const ConcurrentQueue& queue) = delete;
	ConcurrentQueue& operator=(const ConcurrentQueue& queue) = delete;
	
	constexpr ConcurrentQueue(ConcurrentQueue&& queue) noexcept {}
	constexpr ConcurrentQueue& operator=(ConcurrentQueue&& queue) noexcept {}

	void push(T&& element) {
	
		u32 elements = size();

		//Resize if necessary
		if ((elements + 1) == capacity()) {

			u32 newSize = elements + 1 + ResizePolicy::increment(elements);

			if (!resize(newSize)) {
				Log::error("Concurrent Queue", "Failed to resize queue from %d to %d. Pushed element discarded.", elements, newSize);
				return;
			}

		}
	
	}



	T pop() {}
	T waitPop() {}

	u32 size() const noexcept {
	
		if (head < tail) {
			return head + cap - tail;
		} else {
			return head - tail;
		}
	
	}

	u32 capacity() const noexcept {
		return cap;
	}

	bool empty() const noexcept {
		return head == tail;
	}

	bool resize(u32 newSize) {

		u32 elements = size();

		//If we have more elements than the new capacity can hold, no resizing occurs.
		if (newSize < (elements + 1)) {
			return false;
		}
		
		//No reallocation
		if (newSize == cap) {
			return true;
		}

		T* newData = ::operator new(newSize * sizeof(T), alignof(T));

		//Move old data
		if (head < tail) {
				
			std::move(std::begin(data) + tail, std::begin(data) + cap, std::begin(newData));
			std::move(std::begin(data), std::begin(data) + head, std::begin(newData) + cap - tail);

		} else {

			std::move(std::begin(data) + tail, std::begin(data) + head, std::begin(newData));

		}

		//Update container
		::operator delete(data, alignof(T));
		data = newData;
		cap = newSize;
		head = 0;
		tail = elements;
	
	}

private:

	T* data;
	std::atomic_uint32_t cap;
	std::atomic_flag mutating;
	std::atomic_uint32_t head;
	std::atomic_uint32_t tail;

};