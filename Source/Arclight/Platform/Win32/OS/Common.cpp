/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Common.cpp
 */

#include "Common.hpp"
#include "Locale/Unicode.hpp"

#include <Windows.h>


namespace OS {

	void SafeHandle::close() {

		if (valid()) {

			CloseHandle(handle);
			handle = InvalidHandle;

		}

	}

	void SafeHandle::acquire(HandleT handle) {

		close();
		this->handle = handle;

	}



	u32 getSystemError() {
		return GetLastError();
	}

	std::string getSystemMessage(u32 messageID) {

		std::wstring message;
		message.resize(128);

		WORD language = LANG_USER_DEFAULT;

		while (message.size() < 60000) {

			u32 len = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, messageID, language, message.data(), message.size(), nullptr);

			if (len != 0) {
				return OS::String::toUTF8({message.data(), len });
			}

			message.resize(message.size() * message.size());

		}

		return "Unknown error";

	}

	std::string getSystemErrorMessage() {
		return getSystemMessage(getSystemError());
	}

	std::string String::toUTF8(std::wstring_view view) {
		return Unicode::convertString<Unicode::UTF16, Unicode::UTF8>(view);
	}

	std::wstring String::toUTF16(std::string_view view) {
		return Unicode::convertString<Unicode::UTF8, Unicode::UTF16>(view);
	}

}