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
#include <memory>


/*
	Basic process information
*/
struct ProcessInfo {

	inline ProcessInfo() :
		basePriority(0),
		processID(0), parentID(0), threadID(0), moduleID(0),
		handleCount(0), threadCount(0)
	{}

	Path executable;

	u64 basePriority;

	u32 processID;
	u32 parentID;
	u32 threadID;
	u32 moduleID;

	u32 handleCount;
	u32 threadCount;

};

/*
	Parameters required to start a new process
*/
struct ProcessStartInfo {

	Path executable;
	Path workingDirectory;
	std::wstring arguments;

	bool createWindow = true;

};

class Process : public ProcessInfo
{
public:
	
	using HandleT = Handle::HandleT;


	Process();

	Process(Process&&) = default;
	Process& operator=(Process&&) = default;

	bool start();
	bool kill();
	bool waitForExit(u32 milliseconds = -1);

	HandleT getHandle();
	Path getExecutablePath();

	HandleT getMainWindowHandle();
	std::string getMainWindowTitle();

	inline HandleT getHandle() const {
		return processHandle->handle;
	}

	inline Path getExecutablePath() const {
		return executable;
	}

	inline HandleT getMainWindowHandle() const {
		return windowHandle;
	}

	inline std::string getMainWindowTitle()const {
		return windowTitle;
	}

	static u32 getCurrentProcessID();
	static Process getCurrentProcess();
	static std::vector<Process> getProcesses();
	static std::vector<Process> getProcesses(const std::string& name);

	static Process getProcessByID(u32 id);
	static std::vector<u32> getProcessIDs();
	static std::vector<u32> getProcessIDs(const std::string& name);

	static Path getModuleFileName(HandleT handle);
	static Path getProcessFileName(HandleT handle);
	static std::string getWindowTitle(HandleT handle);


	ProcessStartInfo startInfo;

private:

	void createHandles();
	void releaseHandles();

	void openFromInfo(const ProcessInfo& info);
	void openFromID(u32 processID);

	static std::vector<u32> enumerateProcessIDs();
	static std::vector<u32> enumerateProcessIDs(const std::string& name);
	static std::vector<ProcessInfo> enumerateProcessInformation();
	static std::vector<ProcessInfo> enumerateProcessInformation(const std::string& name);

	std::unique_ptr<SafeHandle> processHandle;
	std::unique_ptr<SafeHandle> threadHandle;
	Handle windowHandle;
	std::string windowTitle;

};
