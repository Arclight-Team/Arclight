/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 os.cpp
 */

#include "os.hpp"
#include "registry.hpp"
#include "common/types.hpp"
#include "notification.hpp"
#include "locale/unicode.hpp"
#include "filesystem/fsentry.hpp"
#include "util/log.hpp"

#include <vector>

#include "Windows.h"
#include "Shlwapi.h"
#include "Commctrl.h"


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



bool OS::init() {

	INITCOMMONCONTROLSEX commctrl;
	commctrl.dwSize = sizeof(commctrl);
	commctrl.dwICC = ICC_WIN95_CLASSES | ICC_USEREX_CLASSES | ICC_STANDARD_CLASSES | ICC_PAGESCROLLER_CLASS | ICC_NATIVEFNTCTL_CLASS | ICC_LINK_CLASS | ICC_INTERNET_CLASSES | ICC_DATE_CLASSES | ICC_COOL_CLASSES;

	if (!InitCommonControlsEx(&commctrl)) {
#ifndef ARC_ARCRT_SILENT
		LogE("Runtime") << "Failed to initialize CommCtrl";
#endif
		return false;
	}

	if (CoInitialize(nullptr)) {
#ifndef ARC_ARCRT_SILENT
		LogE("Runtime") << "Failed to initialize COM";
#endif
		return false;
	}

#ifndef ARC_WIN_DISABLE_CONSOLE
	if (!SetConsoleOutputCP(CP_UTF8)) {
#ifndef ARC_ARCRT_SILENT
		LogE("Runtime") << "Console failed to switch to Unicode";
#endif
		return false;
	}
#endif

	return true;

}



void OS::finish() {

	CoUninitialize();
	Notification::purgeAll();

}



OS::Process OS::invoke(const std::string& command, const Path& cwd, bool enableRedirection) {

	ProcessStartInfo info;
	info.executable = "C:\\Windows\\System32\\cmd.exe";
	info.moduleAsArgv0 = true;
	info.attachStdIn = enableRedirection;
	info.attachStdOut = enableRedirection;
	info.attachStdError = enableRedirection;
	info.arguments = "/Q /C \"" + command + "\"";
	info.conMode = ProcessConsoleMode::Invisible;
	info.workingDirectory = cwd;

	Process p;

	if (!p.start(info)) {
		LogE("OS") << "Failed to invoke command '" << command << "'";
	}

	return p;

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