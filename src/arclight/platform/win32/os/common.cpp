/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 common.cpp
 */

#include "common.hpp"

#define NOMINMAX
#include <Windows.h>


void SafeHandle::close() {

	if (valid()) {

		CloseHandle(handle);
		handle = nullptr;

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

	std::string message;

	message.resize(2048);

	WORD language = LANG_USER_DEFAULT;

	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, messageID, language, message.data(), message.size(), nullptr);

	SizeT end = message.find_first_of('\0');

	if (end != message.npos)
		return message.substr(0, end);

	return message;

}

std::string getSystemErrorMessage() {
	return getSystemMessage(getSystemError());
}
