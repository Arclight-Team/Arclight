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
#include <utility>



namespace OS {

	/*
		General purpose handle storage
	*/
	class Handle {

	public:

		using HandleT = void*;

		inline static HandleT InvalidHandle = HandleT(reinterpret_cast<HandleT>(-1));


		Handle() : handle(InvalidHandle) {}
		Handle(HandleT handle) : handle(handle) {}

		// Removes the current handle
		void close() {
			handle = nullptr;
		}

		// Acquires a new handle
		void acquire(HandleT handle) {
			this->handle = handle;
		}

		static bool valid(HandleT handle) {
			return handle != InvalidHandle;
		}

		bool valid() const {
			return valid(handle);
		}

		operator HandleT() const {
			return handle;
		}


		HandleT handle;

	};

	/*
		General purpose handle storage with automatic release on destruction
	*/
	class SafeHandle : public Handle {

	public:

		SafeHandle() = default;
		SafeHandle(HandleT handle) : Handle(handle) {}

		inline ~SafeHandle() {
			close();
		}

		SafeHandle(const SafeHandle&) = delete;
		SafeHandle& operator=(const SafeHandle&) = delete;

		SafeHandle(SafeHandle&& other) noexcept :
			Handle(std::exchange(other.handle, nullptr))
		{}

		SafeHandle& operator=(SafeHandle&& rhs) noexcept {

			this->handle = std::exchange(rhs.handle, nullptr);

			return *this;

		}

		// Closes the current handle and resets the object
		void close();

		// Closes the current handle and acquires a new one
		void acquire(HandleT handle);

	};


	// Returns the latest system error code
	u32 getSystemError();

	// Returns the given error code's system message
	std::string getSystemMessage(u32 messageID);

	// Returns the latest system error message
	std::string getSystemErrorMessage();

	namespace String {

		// Convenience string conversion functions
		std::string toUTF8(std::wstring_view view);
		std::wstring toUTF16(std::string_view view);

	}

}
