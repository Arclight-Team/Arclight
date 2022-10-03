/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 os.cpp
 */

#include "os.hpp"
#include "registry.hpp"
#include "types.hpp"
#include "locale/unicode.hpp"
#include "filesystem/fsentry.hpp"

#include <vector>

#include "Windows.h"
#include "Shlwapi.h"



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



static std::string dispatchPath(Path path) {

	std::string pathStr = path.toAbsolute().toNativeString();

	for (char c : pathStr) {

		if (c == ' ') {

			pathStr.resize(pathStr.size() + 2, '"');
			std::copy_backward(pathStr.begin(), pathStr.end() - 2, pathStr.end() - 1);
			pathStr[0] = '"';
			break;

		}

	}

	return pathStr;

}



bool OS::addToStartup(const std::string& name, const Path& path, bool allUsers) {

	std::optional<OS::Registry::Key> key = OS::Registry::openKey(allUsers ? OS::Registry::RootKey::LocalMachine : OS::Registry::RootKey::CurrentUser, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", OS::Registry::KeyAccess::Write);

	if (key.has_value()) {
		return key->setString(name, dispatchPath(path));
	}

	return false;

}



bool OS::removeFromStartup(const std::string& name, bool allUsers) {

	std::optional<OS::Registry::Key> key = OS::Registry::openKey(allUsers ? OS::Registry::RootKey::LocalMachine : OS::Registry::RootKey::CurrentUser, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", OS::Registry::KeyAccess::Write);

	if (key.has_value()) {
		return key->eraseValue(name);
	}

	return false;

}



bool OS::launch(LaunchAction action, const Path& path, const std::string& params, bool switchCWD) {

	std::wstring verb;

	switch (action) {

		default:
		case LaunchAction::Default: 	break;
		case LaunchAction::Edit:		verb = L"edit";			break;
		case LaunchAction::Explore:		verb = L"explore";		break;
		case LaunchAction::Find:		verb = L"find";			break;
		case LaunchAction::Open:		verb = L"open";			break;
		case LaunchAction::Print:		verb = L"print";		break;
		case LaunchAction::Properties:	verb = L"properties";	break;
		case LaunchAction::RunAsAdmin:	verb = L"runas";		break;

	}


	std::wstring file = Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(path.toNativeString());
	std::wstring parameters = params.empty() ? std::wstring() : Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(params);
	std::wstring wdPath;

	if (switchCWD) {

		Path p = path;
		wdPath = Unicode::convertString<Unicode::UTF8, Unicode::UTF16LE>(FSEntry(path).isFile() ? path.parent().toAbsolute().toNativeString() : p.toAbsolute().toNativeString());

	}

	SHELLEXECUTEINFOW info;
	info.cbSize = sizeof(SHELLEXECUTEINFOW);
	info.fMask = SEE_MASK_DEFAULT | SEE_MASK_NO_CONSOLE;
	info.hwnd = nullptr;
	info.lpVerb = action == LaunchAction::Default ? nullptr : verb.c_str();
	info.lpFile = file.c_str();
	info.lpParameters = params.empty() ? nullptr : parameters.c_str();
	info.lpDirectory = wdPath.c_str();
	info.nShow = SW_NORMAL;

	return ShellExecuteExW(&info);

}