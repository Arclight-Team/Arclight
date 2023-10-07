/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 process.hpp
 */

#pragma once

#include "common.hpp"
#include "filesystem/path.hpp"

#include <string>
#include <vector>
#include <memory>



namespace OS {

	/*
		Basic process information
	*/
	struct ProcessInfo {

		inline ProcessInfo() : basePriority(0), processID(0), parentID(0), threadCount(0) {}

		Path executable;
		u64 basePriority;
		u32 processID;
		u32 parentID;
		u32 threadCount;

	};

	enum class ProcessConsoleMode {

		Inherit,		// Inherit from parent, child process dies with parent
		New,			// New, visible console separate from parent
		Invisible,		// Invisible console separate from parent
		Detached		// No console

	};


	/*
		Parameters required to start a new process
	*/
	struct ProcessStartInfo {

		Path executable;
		Path workingDirectory;
		std::string arguments;
		std::vector<std::string> environment;

		ProcessConsoleMode conMode = ProcessConsoleMode::Inherit;
		bool separateGroup = false;		// Ignored with ProcessConsoleMode::New
		bool moduleAsArgv0 = true;

		bool attachStdIn = false;
		bool attachStdOut = false;
		bool attachStdError = false;

	};

	class Process {

	public:

		constexpr static u32 invalidProcessID = -1;

		using HandleT = Handle::HandleT;

		Process();

		Process(Process&&) = default;
		Process& operator=(Process&&) = default;

		~Process();

		bool start(const ProcessStartInfo& startInfo);
		bool kill();
		bool waitForExit(u32 milliseconds = -1);
		bool active() const;
		bool isCurrent() const;
		void release();
		u32 getResultCode() const;

		std::string readStdout(SizeT maxSize = 32768) const;
		std::string readStderr(SizeT maxSize = 32768) const;
		bool writeStdin(std::string_view str);

		HandleT getHandle();
		Path getExecutablePath();

		HandleT getMainWindowHandle() const;

		inline HandleT getHandle() const {
			return processHandle->handle;
		}

		static u32 getCurrentProcessID();
		static Process getCurrentProcess();
		static Path getCurrentExecutablePath();

		static Process getProcessByID(u32 id);
		static std::vector<u32> getProcessIDs();
		static std::vector<Process> getProcesses();

		static Path getExecutablePath(HandleT handle);

	private:

		void createHandle();
		void openFromID(u32 id);

		static std::string readOutputPipe(HandleT pipe, SizeT maxSize);

		static std::vector<u32> enumerateProcessIDs();
		static std::vector<ProcessInfo> enumerateProcessInformation();

		std::unique_ptr<SafeHandle> processHandle;
		std::unique_ptr<SafeHandle> pipeInHandle;
		std::unique_ptr<SafeHandle> pipeOutHandle;
		std::unique_ptr<SafeHandle> pipeErrorHandle;

		u32 processID;

	};

}