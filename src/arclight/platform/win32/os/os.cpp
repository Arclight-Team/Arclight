/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 os.cpp
 */

#include "os.hpp"
#include "types.hpp"
#include "locale/unicode.hpp"

#include <vector>

#include "Windows.h"



std::optional<std::string> OS::Environment::getVariable(const std::string& var) {

	std::wstring envVar = Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(var);
	std::vector<WCHAR> buffer(256);

	SizeT bufferSize = GetEnvironmentVariableW(envVar.c_str(), buffer.data(), buffer.size());

	if (bufferSize >= buffer.size()) {

		buffer.resize(bufferSize);
		bufferSize = GetEnvironmentVariableW(envVar.c_str(), buffer.data(), buffer.size());

	}

	return bufferSize ? Unicode::convertString<Unicode::UTF16LE, Unicode::UTF8>(std::wstring_view(buffer.data(), buffer.data() + bufferSize)) : std::optional<std::string>{};

}



bool OS::Environment::setVariable(const std::string& var, const std::string& contents) {
	return SetEnvironmentVariableW(Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(var).c_str(), Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(contents).c_str());
}



static bool getShutdownPriviledge() {

	HANDLE handle;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle)) {
		return false;
	}

 	TOKEN_PRIVILEGES tokenPrivileges;

	if (!LookupPrivilegeValueW(nullptr, SE_SHUTDOWN_NAME, &tokenPrivileges.Privileges[0].Luid)) {
		return false;
	}

	tokenPrivileges.PrivilegeCount = 1;
	tokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	return AdjustTokenPrivileges(handle, false, &tokenPrivileges, 0, nullptr, 0);

}



bool OS::logout() {
	return ExitWindowsEx(EWX_LOGOFF | EWX_FORCEIFHUNG, SHTDN_REASON_MINOR_OTHER);
}



bool OS::restart(bool force) {

	if (!getShutdownPriviledge()) {
		return false;
	}

	u32 forceFlags = force ? EWX_FORCE : 0;
	return ExitWindowsEx(EWX_REBOOT | EWX_RESTARTAPPS | EWX_FORCEIFHUNG | forceFlags, SHTDN_REASON_MINOR_OTHER);

}



bool OS::shutdown(bool force) {

	if (!getShutdownPriviledge()) {
		return false;
	}

	u32 forceFlags = force ? EWX_FORCE : 0;
	return ExitWindowsEx(EWX_POWEROFF | EWX_FORCEIFHUNG | forceFlags, SHTDN_REASON_MINOR_OTHER);

}