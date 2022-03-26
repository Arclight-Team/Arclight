/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 process.cpp
 */

#include "process.hpp"
#include "util/log.hpp"
#include "util/bits.hpp"
#include "util/bool.hpp"

#include <functional>

#define NOMINMAX
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>


namespace Detail
{

	using EnumerateProcessCallback = std::function<bool(const PROCESSENTRY32W&)>;
	
	/*
		Enumerate all processes and invoke a callback for each entry
	*/
	void enumerateProcesses(const EnumerateProcessCallback& callback) {

		// Create process snapshot
		SafeHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0));

		if (!snapshot.valid()) {

			Log::error("Process", "Failed to create snapshot of all processes:");
			Log::error("Process", getSystemErrorMessage());

			return;
		}

		PROCESSENTRY32 pe32{};
		pe32.dwSize = sizeof(pe32);

		// Fetch first process
		if (!Process32First(snapshot, &pe32)) {

			Log::error("Process", "Failed to fetch first process:");
			Log::error("Process", getSystemErrorMessage());

			return;
		}

		while (true) {

			if (callback(pe32))
				break;

			// Fetch next process
			if (!Process32Next(snapshot, &pe32))
				break;

		}

	}

}



Process::Process() {
	createHandles();
}

bool Process::start() {

	if (startInfo.executable.isEmpty()) {
		Log::error("Process", "Executable path is empty");
		return false;
	}

	std::wstring commandLine;
	commandLine = startInfo.executable.getHandle();
	commandLine += L" " + startInfo.arguments;

	std::wstring currentDirectory = startInfo.workingDirectory.getHandle();

	if (startInfo.workingDirectory.isEmpty())
		currentDirectory = Path::getCurrentWorkingDirectory().getHandle();

	u32 creationFlags = 0;
	if (!startInfo.createWindow)
		creationFlags |= CREATE_NO_WINDOW;

	void* environment = nullptr;

	STARTUPINFO si{};
	si.cb = sizeof(si);

	PROCESS_INFORMATION pi{};

	if (!CreateProcessW(nullptr, commandLine.data(), 0, 0, true, creationFlags, environment, currentDirectory.c_str(), &si, &pi)) {
		Log::error("Process", "Failed to start process:");
		Log::error("Process", getSystemErrorMessage());
		return false;
	}

	processHandle->acquire(pi.hProcess);
	threadHandle->acquire(pi.hThread);
	windowHandle.close();

	executable = getProcessFileName(pi.hProcess);
	processID = pi.dwProcessId;
	threadID = pi.dwThreadId;

	return true;

}

bool Process::kill() {

	if (!processHandle->valid())
		return false;

	if (!TerminateProcess(*processHandle, -1)) {
		
		Log::error("Process", "Failed to kill process:");
		Log::error("Process", getSystemErrorMessage());

		releaseHandles();

		return false;
	}

	releaseHandles();

	return true;

}

bool Process::waitForExit(u32 milliseconds) {

	if (!processHandle->valid())
		return false;

	if (WaitForSingleObject(*processHandle, milliseconds) != WAIT_OBJECT_0)
		return false;

	releaseHandles();

	return true;
}

Process::HandleT Process::getHandle() {

	if (processHandle->valid())
		return *processHandle;

	if (!processID)
		return nullptr;

	u32 access = PROCESS_ALL_ACCESS;

	SafeHandle handle(OpenProcess(access, false, processID));

	if (!handle.valid()) {

		u32 err = getSystemError();

		// PID is not valid or the process requires higher permissions
		if (Bool::any(err, ERROR_ACCESS_DENIED, ERROR_INVALID_PARAMETER))
			return nullptr;

		Log::error("Process", "Failed to open process from ID:");
		Log::error("Process", getSystemErrorMessage());
	
		return nullptr;
	}

	processHandle->close();
	*processHandle = std::move(handle);
	
	return *processHandle;

}

Path Process::getExecutablePath() {

	if (!executable.isEmpty())
		return executable;

	HandleT handle = getHandle();

	executable = getProcessFileName(handle);

	return executable;

}

Process::HandleT Process::getMainWindowHandle() {

	if (windowHandle.valid())
		return windowHandle;

	static Handle findWindowHandle;
	static u32 findWindowProcessID;

	findWindowHandle.close();
	findWindowProcessID = processID;

	auto result = EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {

		DWORD pid;
		GetWindowThreadProcessId(hWnd, &pid);

		// Window is not owned by the process
		if (pid != findWindowProcessID)
			return true;

		// Not a main window
		if (!GetWindow(hWnd, GW_OWNER))
			return true;

		findWindowHandle.acquire(hWnd);

		return false;

	}, 0);

	if (!findWindowHandle.valid()) {
		return nullptr;
	}

	windowHandle = findWindowHandle;

	return windowHandle;

}

std::string Process::getMainWindowTitle() {

	if (!windowTitle.empty())
		return windowTitle;

	HandleT window = getMainWindowHandle();

	if (!window)
		return "";

	windowTitle = getWindowTitle(window);

	return windowTitle;

}

u32 Process::getCurrentProcessID() {
	return GetCurrentProcessId();
}

std::vector<u32> Process::getProcessIDs() {
	return enumerateProcessIDs();
}

std::vector<u32> Process::getProcessIDs(const std::string& name) {
	return enumerateProcessIDs(name);
}

Process Process::getCurrentProcess() {
	return getProcessByID(getCurrentProcessID());
}

Process Process::getProcessByID(u32 id) {

	Process process;

	process.openFromID(id);

	return process;

}

std::vector<Process> Process::getProcesses() {

	std::vector<Process> processes;

	auto list = enumerateProcessInformation();

	processes.resize(list.size());

	for (u32 i = 0; i < list.size(); i++) {

		processes[i].openFromInfo(list[i]);
		
	}

	return processes;

}

std::vector<Process> Process::getProcesses(const std::string& name) {

	std::vector<Process> processes;

	auto list = enumerateProcessInformation(name);

	processes.resize(list.size());

	for (u32 i = 0; i < list.size(); i++) {

		processes[i].openFromInfo(list[i]);

	}

	return processes;

}

Path Process::getModuleFileName(HandleT handle) {

	std::wstring path;

	path.resize(2048);

	if (!GetModuleFileNameW(HMODULE(handle), path.data(), path.size())) {

		Log::error("Process", "Could not fetch module name:");
		Log::error("Process", getSystemErrorMessage());

		return {};
	}

	SizeT end = path.find_first_of(L'\0');

	if (end != path.npos)
		path = path.substr(0, end);

	return Path(path);

}

Path Process::getProcessFileName(HandleT handle) {

	std::wstring path;

	path.resize(2048);

	if (!GetModuleBaseNameW(handle, nullptr, path.data(), path.size())) {

		Log::error("Process", "Could not fetch process name:");
		Log::error("Process", getSystemErrorMessage());

		return {};
	}

	SizeT end = path.find_first_of(L'\0');

	if (end != path.npos)
		path = path.substr(0, end);

	return Path(path);

}

std::string Process::getWindowTitle(HandleT handle) {

	u32 length = GetWindowTextLengthA(HWND(handle)) + 1;

	std::string title;

	title.resize(length);

	GetWindowTextA(HWND(handle), title.data(), title.size());

	return title;

}



void Process::createHandles() {

	processHandle = std::make_unique<SafeHandle>();
	threadHandle = std::make_unique<SafeHandle>();
	windowHandle.close();

}

void Process::releaseHandles() {

	processHandle->close();
	threadHandle->close();
	windowHandle.close();

}

void Process::openFromInfo(const ProcessInfo& info) {
	
	releaseHandles();

	executable = info.executable;
	basePriority = info.basePriority;
	processID = info.processID;
	parentID = info.parentID;
	threadID = info.threadID;
	moduleID = info.moduleID;
	handleCount = info.handleCount;
	threadCount = info.threadCount;

	// Fetch process handle from process ID
	getHandle();

	// Fetch main window handle and title
	getMainWindowTitle();

}

void Process::openFromID(u32 processID) {

	releaseHandles();

	this->processID = processID;

	// Fetch process handle from process ID
	HandleT handle = getHandle();

	if (!handle)
		return;

	// Fetch executable path from process handle
	getExecutablePath();

	// Fetch main window handle and title
	getMainWindowTitle();

}

std::vector<u32> Process::enumerateProcessIDs() {

	std::vector<u32> list;

	Detail::enumerateProcesses([&](const PROCESSENTRY32W& pe32) {

		list.push_back(pe32.th32ProcessID);

		return false;
	});

	return list;

}

std::vector<u32> Process::enumerateProcessIDs(const std::string& name) {

	std::vector<u32> list;

	Detail::enumerateProcesses([&](const PROCESSENTRY32W& pe32) {

		std::wstring str;
		str.resize(MAX_PATH);
		str.assign(pe32.szExeFile);
		str.shrink_to_fit();

		Path path(str);

		// TODO: case insensitive compare!!!!
		if (path.getStem() == name) {
			list.push_back(pe32.th32ProcessID);
		}

		return false;
	});

	return list;

}

std::vector<ProcessInfo> Process::enumerateProcessInformation() {

	std::vector<ProcessInfo> list;

	Detail::enumerateProcesses([&](const PROCESSENTRY32W& pe32) {

		std::wstring str;
		str.resize(MAX_PATH);
		str.assign(pe32.szExeFile);
		str.shrink_to_fit();

		ProcessInfo& pi = list.emplace_back();

		pi.executable = Path(str);
		pi.processID = pe32.th32ProcessID;
		pi.parentID = pe32.th32ParentProcessID;
		pi.moduleID = pe32.th32ModuleID;
		pi.handleCount = pe32.cntUsage;
		pi.threadCount = pe32.cntThreads;
		pi.basePriority = pe32.pcPriClassBase;

		return false;
	});

	return list;

}

std::vector<ProcessInfo> Process::enumerateProcessInformation(const std::string& name) {

	std::vector<ProcessInfo> list;

	Detail::enumerateProcesses([&](const PROCESSENTRY32W& pe32) {

		std::wstring str;
		str.resize(MAX_PATH);
		str.assign(pe32.szExeFile);
		str.shrink_to_fit();

		Path path(str);

		// TODO: case insensitive compare!!!!
		if (path.getStem() == name) {

			ProcessInfo& pi = list.emplace_back();

			pi.executable = path;
			pi.processID = pe32.th32ProcessID;
			pi.parentID = pe32.th32ParentProcessID;
			pi.moduleID = pe32.th32ModuleID;
			pi.handleCount = pe32.cntUsage;
			pi.threadCount = pe32.cntThreads;
			pi.basePriority = pe32.pcPriClassBase;

		}

		return false;
	});

	return list;

}
