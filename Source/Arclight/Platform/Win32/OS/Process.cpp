/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Process.cpp
 */

#include "Process.hpp"
#include "Util/Log.hpp"
#include "Util/Bits.hpp"
#include "Util/Bool.hpp"
#include "Util/DestructionGuard.hpp"
#include "Util/String.hpp"
#include "Common/Win32.hpp"

#include <functional>

#include <TlHelp32.h>
#include <Psapi.h>



namespace OS {

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

				LogE("Process") << "Failed to create snapshot of all processes:";
				LogE("Process") << OS::getSystemErrorMessage();

				return;
			}

			PROCESSENTRY32W pe32{};
			pe32.dwSize = sizeof(pe32);

			// Fetch first process
			if (!Process32FirstW(snapshot, &pe32)) {

				LogE("Process") << "Failed to fetch first process:";
				LogE("Process") << OS::getSystemErrorMessage();

				return;
			}

			while (true) {

				if (callback(pe32))
					break;

				// Fetch next process
				if (!Process32NextW(snapshot, &pe32))
					break;

			}

		}

	}



	Process::Process() : processID(invalidProcessID) {
		createHandle();
	}

	Process::~Process() {
		release();
	}


	bool Process::start(const ProcessStartInfo& startInfo) {

		if (startInfo.executable.isEmpty()) {

			LogE("Process") << "Executable path is empty";
			return false;

		}

		std::wstring executable = startInfo.executable.getHandle();
		std::wstring commandLine;

		if (startInfo.moduleAsArgv0) {

			std::string quoted = ::String::quote(startInfo.executable.toNativeString());

			if (quoted.size() >= MAX_PATH) {

				LogW("Process") << "Resulting module path exceeds MAX_PATH in argv[0], potentially unreliable";
				quoted.resize(MAX_PATH);

			}

			std::string fullCMDL = quoted + " " + startInfo.arguments;
			commandLine = OS::String::toUTF16(fullCMDL);

		} else {

			commandLine = OS::String::toUTF16(startInfo.arguments);

		}

		std::wstring cwd;
		const wchar_t* cwdPath = nullptr;

		if (!startInfo.workingDirectory.isEmpty()) {

			cwd = startInfo.workingDirectory.getHandle();
			cwdPath = cwd.c_str();
		}

		std::wstring environmentArray;
		wchar_t* env = nullptr;

		if (!startInfo.environment.empty()) {

			std::vector<std::wstring> envSliced;
			envSliced.reserve(startInfo.environment.size());

			SizeT totalSize = 0;

			for (const std::string& s : startInfo.environment) {

				envSliced.emplace_back(OS::String::toUTF16(s));
				totalSize += envSliced.back().size() + 1;

			}

			// We already get the null terminator for free
			environmentArray.resize(totalSize);
			SizeT i = 0;

			for (const std::wstring& w : envSliced) {

				std::copy(w.begin(), w.end(), environmentArray.begin() + i);
				i += w.size() + 1;

			}

			env = environmentArray.data();

		}

		u32 creationFlags = CREATE_UNICODE_ENVIRONMENT;

		switch (startInfo.conMode) {

			case ProcessConsoleMode::New:		creationFlags |= CREATE_NEW_CONSOLE;	break;
			case ProcessConsoleMode::Invisible:	creationFlags |= CREATE_NO_WINDOW;		break;
			case ProcessConsoleMode::Detached:	creationFlags |= DETACHED_PROCESS;		break;
			default:	break;

		}

		if (startInfo.separateGroup) {
			creationFlags |= CREATE_NEW_PROCESS_GROUP;
		}

		STARTUPINFOW si{};
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(si);
		si.dwFlags |= STARTF_USESTDHANDLES;

		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = true;
		sa.lpSecurityDescriptor = nullptr;

		SafeHandle pipeInRead;
		SafeHandle pipeInWrite;
		SafeHandle pipeOutRead;
		SafeHandle pipeOutWrite;
		SafeHandle pipeErrorRead;
		SafeHandle pipeErrorWrite;

		if (startInfo.attachStdIn) {

			HandleT stdInPipeWrite;
			HandleT stdInPipeRead;

			if (!CreatePipe(&stdInPipeRead, &stdInPipeWrite, &sa, 0)) {

				LogE("Process") << "Failed to redirect stdin";
				return false;

			}

			pipeInRead.acquire(stdInPipeRead);
			pipeInWrite.acquire(stdInPipeWrite);

			if (!SetHandleInformation(stdInPipeWrite, HANDLE_FLAG_INHERIT, 0)) {

				LogE("Process") << "Failed to redirect stdin";
				return false;

			}

			si.hStdInput = stdInPipeRead;

		}

		if (startInfo.attachStdOut) {

			HandleT stdOutPipeWrite;
			HandleT stdOutPipeRead;

			if (!CreatePipe(&stdOutPipeRead, &stdOutPipeWrite, &sa, 0)) {

				LogE("Process") << "Failed to redirect stdout";
				return false;

			}

			pipeOutRead.acquire(stdOutPipeRead);
			pipeOutWrite.acquire(stdOutPipeWrite);

			if (!SetHandleInformation(stdOutPipeRead, HANDLE_FLAG_INHERIT, 0)) {

				LogE("Process") << "Failed to redirect stdout";
				return false;

			}

			si.hStdOutput = stdOutPipeWrite;

		}

		if (startInfo.attachStdError) {

			HandleT stdErrorPipeWrite;
			HandleT stdErrorPipeRead;

			if (!CreatePipe(&stdErrorPipeRead, &stdErrorPipeWrite, &sa, 0)) {

				LogE("Process") << "Failed to redirect stderr";
				return false;

			}

			pipeErrorRead.acquire(stdErrorPipeRead);
			pipeErrorWrite.acquire(stdErrorPipeWrite);

			if (!SetHandleInformation(stdErrorPipeRead, HANDLE_FLAG_INHERIT, 0)) {

				LogE("Process") << "Failed to redirect stderr";
				return false;

			}

			si.hStdError = stdErrorPipeWrite;

		}

		PROCESS_INFORMATION pi{};
		ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		if (!CreateProcessW(executable.c_str(), commandLine.data(), nullptr, nullptr, true, creationFlags, env, cwdPath, &si, &pi)) {

			LogE("Process") << "Failed to start process:";
			LogE("Process") << OS::getSystemErrorMessage();
			return false;

		}

		CloseHandle(pi.hThread);

		processHandle->acquire(pi.hProcess);
		processID = pi.dwProcessId;

		if (pipeInWrite.valid()) {
			pipeInHandle = std::make_unique<SafeHandle>(std::move(pipeInWrite));
		}

		if (pipeOutRead.valid()) {
			pipeOutHandle = std::make_unique<SafeHandle>(std::move(pipeOutRead));
		}

		if (pipeErrorRead.valid()) {
			pipeErrorHandle = std::make_unique<SafeHandle>(std::move(pipeErrorRead));
		}

		return true;

	}



	bool Process::kill() {

		if (!processHandle->valid())
			return false;

		if (!TerminateProcess(*processHandle, -1)) {

			LogE("Process") << "Failed to kill process:";
			LogE("Process") << OS::getSystemErrorMessage();

			release();

			return false;
		}

		release();

		return true;

	}



	bool Process::waitForExit(u32 milliseconds) {

		if (!processHandle->valid()) {
			return false;
		}

		if (WaitForSingleObject(*processHandle, milliseconds) != WAIT_OBJECT_0) {
			return false;
		}

		return true;

	}



	bool Process::active() const {

		DWORD res = 0;
		return processHandle->valid() && GetExitCodeProcess(processHandle->handle, &res) && res == STILL_ACTIVE;

	}



	bool Process::isCurrent() const {
		return processHandle && processID == getCurrentProcessID();
	}



	void Process::release() {

		processHandle->close();

		pipeInHandle.reset();
		pipeOutHandle.reset();
		pipeErrorHandle.reset();

		processID = invalidProcessID;

	}



	u32 Process::getResultCode() const {

		if (!processHandle->valid()) {

			LogE("Process") << "Attempted to obtain result code from invalid process";
			return -1;

		}

		DWORD res = 0;

		if (!GetExitCodeProcess(processHandle->handle, &res)) {

			LogE("Process") << "Failed to obtain result code from process";
			return -1;

		}

		return res;

	}



	std::string Process::readStdout(SizeT maxSize) const {

		if (!pipeOutHandle) {

			LogE("Process") << "Stdout pipe has not been opened";
			return "";

		}

		return readOutputPipe(pipeOutHandle->handle, maxSize);

	}



	std::string Process::readStderr(SizeT maxSize) const {

		if (!pipeErrorHandle) {

			LogE("Process") << "Stderr pipe has not been opened";
			return "";

		}

		return readOutputPipe(pipeErrorHandle->handle, maxSize);

	}



	bool Process::writeStdin(std::string_view str) {

		if (!pipeInHandle) {

			LogE("Process") << "Stdin pipe has not been opened";
			return false;

		}

		DWORD written = 0;
		return WriteFile(pipeInHandle->handle, str.data(), str.size(), &written, nullptr);

	}



	Process::HandleT Process::getHandle() {
		return processHandle->valid() ? processHandle->handle : nullptr;
	}



	Path Process::getExecutablePath() {

		HandleT handle = getHandle();

		return handle ? (isCurrent() ? getCurrentExecutablePath() : getExecutablePath(handle)) : Path{};

	}



	Process::HandleT Process::getMainWindowHandle() const {

		thread_local Handle findWindowHandle;
		thread_local u32 findWindowProcessID;

		findWindowHandle.close();
		findWindowProcessID = processID;

		auto result = EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {

			DWORD pid = 0;
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

		return findWindowHandle;

	}



	u32 Process::getCurrentProcessID() {
		return GetCurrentProcessId();
	}



	Process Process::getCurrentProcess() {
		return getProcessByID(getCurrentProcessID());
	}



	Path Process::getCurrentExecutablePath() {

		u32 length = 0x100;
		std::wstring filename(length, L'\0');

		while (GetModuleFileNameW(nullptr, filename.data(), length) == length) {

			if(length < 0x8000) {

				length *= 2;
				filename.resize(length);

			} else {

				return {};

			}

		}

		return Path(Unicode::convertString<Unicode::UTF16LE, Unicode::UTF8>(filename.data()));

	}


	Process Process::getProcessByID(u32 id) {

		Process process;
		process.openFromID(id);

		return process;

	}



	std::vector<u32> Process::getProcessIDs() {
		return enumerateProcessIDs();
	}



	std::vector<Process> Process::getProcesses() {

		std::vector<ProcessInfo> list = enumerateProcessInformation();
		std::vector<Process> processes(list.size());

		for (SizeT i = 0; i < list.size(); i++) {
			processes[i] = getProcessByID(list[i].processID);
		}

		return processes;

	}



	Path Process::getExecutablePath(HandleT handle) {

		u32 length = 0x100;
		std::wstring filename(length, L'\0');

		while (true) {

			DWORD len = filename.size();

			if (!QueryFullProcessImageNameW(handle, 0, filename.data(), &len)) {

				LogE("Process") << "Could not fetch executable path:";
				LogE("Process") << OS::getSystemErrorMessage();

				return {};

			}

			if (len != filename.size()) {

				filename.resize(len);
				break;

			}

			if (filename.size() >= 32768) {
				return {};
			}

			filename.resize(filename.size() * 2);

		}

		return Path(OS::String::toUTF8(filename));

	}



	void Process::createHandle() {
		processHandle = std::make_unique<SafeHandle>();
	}



	void Process::openFromID(u32 id) {

		release();

		// Fetch process handle from process ID
		HandleT handle = OpenProcess(PROCESS_ALL_ACCESS, false, processID);

		if (!handle)
			return;

		processID = id;
		processHandle->acquire(handle);

	}



	std::string Process::readOutputPipe(HandleT pipe, SizeT maxSize) {

		constexpr SizeT bufferSize = 2048;
		std::string str;

		SizeT offset = 0;
		i64 maxRead = maxSize;

		while (maxRead > 0) {

			SizeT toRead = maxSize < bufferSize ? maxSize : bufferSize;
			str.resize(str.size() + toRead);

			DWORD read = 0;
			bool res = ReadFile(pipe, str.data() + offset, toRead, &read, nullptr);

			if (!res) {

				LogE("Process") << "Stdout pipe broken";
				LogE("Process") << OS::getSystemErrorMessage();

				str.resize(str.size() - toRead);
				return str;

			} else if (read != toRead) {

				str.resize(str.size() - (toRead - read));
				return str;

			}

			offset += read;
			maxRead -= read;

		}

		return str;

	}



	std::vector<u32> Process::enumerateProcessIDs() {

		std::vector<u32> list;

		Detail::enumerateProcesses([&](const PROCESSENTRY32W& pe32) {
			list.push_back(pe32.th32ProcessID);
			return false;
		});

		return list;

	}



	std::vector<ProcessInfo> Process::enumerateProcessInformation() {

		std::vector<ProcessInfo> list;

		Detail::enumerateProcesses([&](const PROCESSENTRY32W& pe32) {

			HandleT handle = OpenProcess(PROCESS_QUERY_INFORMATION, false, pe32.th32ProcessID);

			// Handle expired
			if (!handle) {
				return false;
			}

			ProcessInfo pi;
			pi.executable = getExecutablePath(handle);

			CloseHandle(handle);

			pi.processID = pe32.th32ProcessID;
			pi.parentID = pe32.th32ParentProcessID;
			pi.threadCount = pe32.cntThreads;
			pi.basePriority = pe32.pcPriClassBase;

			list.emplace_back(pi);

			return false;

		});

		return list;

	}

}