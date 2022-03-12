/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 common.hpp
 */

#pragma once

#include "types.hpp"
#include <string>


/*
	General purpose handle storage
*/
class Handle
{
public:

	using HandleT = void*;

	constexpr static HandleT InvalidHandle = HandleT(-1);


	constexpr Handle() : handle(0) {}
	constexpr Handle(HandleT handle) : handle(handle) {}

	// Removes the current handle
	constexpr void close() {
		handle = nullptr;
	}

	// Acquires a new handle
	constexpr void acquire(HandleT handle) {
		this->handle = handle;
	}

	constexpr static bool valid(HandleT handle) {
		return handle && handle != InvalidHandle;
	}

	constexpr bool valid() const {
		return valid(handle);
	}

	constexpr operator HandleT() const {
		return handle;
	}


	HandleT handle;
	
};

/*
	General purpose handle storage with automatic release on destruction
*/
class SafeHandle : public Handle {
public:

	constexpr SafeHandle() = default;
	constexpr SafeHandle(HandleT handle) : Handle(handle) {}

	inline ~SafeHandle() {
		close();
	}

	SafeHandle(const SafeHandle&) = delete;
	SafeHandle& operator=(const SafeHandle&) = delete;

	constexpr SafeHandle(SafeHandle&& other) noexcept :
		Handle(std::exchange(other.handle, nullptr))
	{}

	constexpr SafeHandle& operator=(SafeHandle&& rhs) noexcept {

		this->handle = std::exchange(rhs.handle, nullptr);

		return *this;

	}

	// Closes the current handle and resets the object
	void close();

	// Closes the current handle and acquires a new one
	void acquire(HandleT handle);

};

/*
	Accessing system errors and localized messages
*/

u32 getSystemError();

std::string getSystemMessage(u32 messageID);

std::string getSystemErrorMessage();
