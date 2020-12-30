#pragma once

#include <atomic>
#include "types.h"


struct StandardResizePolicy {

	static constexpr u32 increment(u32 base) {
		return base + 1;
	}

};


template<class T, u32 InitialSize = 32, u32 MaxSize = 513, class ResizePolicy = StandardResizePolicy>
class ConcurrentQueue final {

public:

	static_assert(std::is_default_constructible_v<T>, "ConcurrentQueue requires T to be default-constructible");
	static_assert(std::is_move_constructible_v<T> && std::is_move_assignable_v<T>, "ConcurrentQueue requires T to be move-constructible and move-assignable");

	constexpr ConcurrentQueue() : ConcurrentQueue(InitialSize) {}

	constexpr ConcurrentQueue(u32 size) {

		initialAllocate(size);
		head.store(0);
		tail.store(0);

	}

	~ConcurrentQueue() {

		if (data) {

			u32 capSize = capacity();
			u32 headIndex = head.load();
			u32 tailIndex = tail.load();

			//Invoke the destructor on the remaining elements
			if (tailIndex < headIndex) {
				std::destroy(data + headIndex, data + capSize);
				std::destroy(data, data + tailIndex);
			} else {
				std::destroy(data + headIndex, data + tailIndex);
			}

			//Finally destroy the storage
			::operator delete(data, std::align_val_t(alignof(T)));
			data = nullptr;

		}

	}

	ConcurrentQueue(const ConcurrentQueue& queue) = delete;
	ConcurrentQueue& operator=(const ConcurrentQueue& queue) = delete;

	constexpr ConcurrentQueue(ConcurrentQueue&& queue) noexcept : 
		data(std::exchange(queue.data, nullptr)),
		cap(queue.cap.load()),
		head(queue.head.load()),
		tail(queue.tail.load()) {}


	constexpr ConcurrentQueue& operator=(ConcurrentQueue&& queue) noexcept {
	
		this->data = std::exchange(queue.data, nullptr);
		this->cap = queue.cap.load();
		this->head = queue.head.load();
		this->tail = queue.tail.load();

	}

	bool push(T&& element) {

		u32 elements = size();

		//Resize if necessary
		if ((elements + 1) == capacity()) {

			u32 newSize = elements + 1 + ResizePolicy::increment(elements);

			if (!resize(newSize)) {
				return false;
			}

		}

		u32 tailIndex = tail.load();

		//Push to tail
		::new(data + tailIndex) T(std::move(element));

		//Increment tail
		tail.store(indexWrap(tailIndex));

		return true;

	}



	T pop() {
	
		if (!empty()) {

			u32 headIndex = head.load();

			//Pop from head
			T&& element = std::move(data[headIndex]);

			//Increment head
			head.store(indexWrap(headIndex));

			//Return the object
			return element;

		} else {

			//Return a default-constructed element
			return T();

		}

	}



	bool tryPop(T& element) {

		if (!empty()) {

			u32 headIndex = head.load();

			//Pop from head to element
			element = std::move(data[headIndex]);

			//Increment head
			head.store(indexWrap(headIndex));

			//Signal that element has been modified
			return true;

		} else {
			//Signal that element has not been modified
			return false;
		}

	}



	u32 size() const noexcept {

		u32 headIndex = head.load();
		u32 tailIndex = tail.load();
		u32 capSize = cap.load();
	
		if (headIndex < tailIndex) {
			return tailIndex - headIndex;
		} else {
			return tailIndex + capSize - headIndex;
		}
	
	}



	u32 capacity() const noexcept {
		return cap.load();
	}



	bool empty() const noexcept {
		return head.load() == tail.load();
	}



	bool resize(u32 newSize) {

		u32 elements = size();
		u32 capSize = capacity();

		//Check if the new size exceeds the given maximum size
		if (newSize > MaxSize) {

			//If we can still grow, resize to maximum
			if (capSize != MaxSize) {
				Log::debug("Concurrent Queue", "Queue cannot grow bigger than %d, resizing to maximum size. Requested: %d.", MaxSize, newSize);
				newSize = MaxSize;
			} else {
				Log::debug("Concurrent Queue", "Queue cannot grow bigger than %d. Requested size: %d.", MaxSize, newSize);
				return false;
			}

		}

		//If we have more elements than the new capacity can hold, no resizing occurs.
		if (newSize < (elements + 1)) {
			Log::debug("Concurrent Queue", "Failed to resize queue from %d to %d. Elements cannot be discarded to accommodate the new size.", elements, newSize);
			return false;
		}
		
		u32 headIndex = head.load();
		u32 tailIndex = tail.load();

		//No reallocation
		if (newSize == capSize) {
			return true;
		}

		T* newData = static_cast<T*>(::operator new(newSize * sizeof(T), std::align_val_t(alignof(T))));

		//Move old data
		if (tailIndex < headIndex) {
				
			std::uninitialized_move(data + headIndex, data + capSize, newData);
			std::uninitialized_move(data, data + tailIndex, newData + capSize - headIndex);

		} else {

			std::uninitialized_move(data + headIndex, data + tailIndex, newData);

		}

		//Update container
		::operator delete(data, std::align_val_t(alignof(T)));
		data = newData;
		cap = newSize;
		head.store(0);
		tail.store(elements);

		return true;
	
	}

private:

	//Performs the initial allocation
	void initialAllocate(u32 size) {

		data = static_cast<T*>(::operator new(size * sizeof(T), std::align_val_t(alignof(T))));
		cap.store(size);

	}

	//Returns the next index on push/pop
	u32 indexWrap(u32 index) {
		return (index == (capacity() - 1)) ? 0 : (index + 1);
	}

	T* data;
	std::atomic_uint32_t cap;
	std::atomic_flag mutating;
	std::atomic_uint32_t head;
	std::atomic_uint32_t tail;

};