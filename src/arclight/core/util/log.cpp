/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 log.cpp
 */

#include "log.hpp"
#include "assert.hpp"
#include "filesystem/file.hpp"
#include "time/time.hpp"
#include "arcconfig.hpp"

#include <iostream>
#include <sstream>


#define LOG_DEBUG "D"
#define LOG_INFO  "I"
#define LOG_WARN  "W"
#define LOG_ERROR "E"



namespace Log {

	File logfile;


	void init() {
#ifdef ARC_LOG_STDIO_UNSYNC
		std::ios_base::sync_with_stdio(false);
#endif
	}


	void openLogFile(Uri path) {

		bool dirCreated = path.createDirectory();

		if (!dirCreated) {
			Log::error("Log", "Failed to create log file directory '%s'", path.getPath().c_str());
			return;
		}

		path.move("log_" + Time::getTimestamp() + ".txt");
		logfile.open(path, File::Out);

		if (!logfile.isOpen()) {
			Log::error("Log", "Failed to open log file '%s'", path.getPath().c_str());
		}

	}


	void closeLogFile() {
		logfile.close();
	}


	void print(const std::string& level, const std::string& subsystem, const std::string& message) noexcept {

		try {

			std::string line = "[" + level + ": " + subsystem + "] " + message;
			std::cout << line << std::endl;

			if (logfile.isOpen()) {
				logfile.writeLine(line);
			}

		} catch (std::exception&) {
			//There's literally nothing we can do here
#if defined(ARC_LOG_EXCEPTION_ABORT) && !defined(ARC_FINAL_BUILD)
			arc_abort();
#endif
		}

	}

	void debug(const std::string& subsystem, const std::string& message) noexcept {
		print(LOG_DEBUG, subsystem, message);
	}


	void info(const std::string& subsystem, const std::string& message) noexcept {
		print(LOG_INFO, subsystem, message);
	}


	void warn(const std::string& subsystem, const std::string& message) noexcept {
		print(LOG_WARN, subsystem, message);
	}


	void error(const std::string& subsystem, const std::string& message) noexcept {
		print(LOG_ERROR, subsystem, message);
	}

}