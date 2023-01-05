/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 common.cpp
 */

#include "common.hpp"
#include "locale/unicode.hpp"

#include <Windows.h>


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



u32 OS::getSystemError() {
	return GetLastError();
}

std::string OS::getSystemMessage(u32 messageID) {

	std::wstring message;
	message.resize(128);

	WORD language = LANG_USER_DEFAULT;

	while (message.size() < 60000) {

		u32 len = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, messageID, language, message.data(), message.size(), nullptr);

		if (len != 0) {
			return Unicode::convertString<Unicode::UTF16, Unicode::UTF8>({ message.data(), len });
		}

		message.resize(message.size() * message.size());

	}

	return "Unknown error";

}

std::string OS::getSystemErrorMessage() {
	return getSystemMessage(getSystemError());
}
