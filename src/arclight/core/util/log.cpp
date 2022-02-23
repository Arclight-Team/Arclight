/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 log.cpp
 */

#include "log.hpp"
#include "assert.hpp"
#include "filesystem/directory.hpp"
#include "filesystem/file.hpp"
#include "time/time.hpp"
#include "arcconfig.hpp"

#include <iostream>

#define LOG_DEBUG "D"
#define LOG_INFO  "I"
#define LOG_WARN  "W"
#define LOG_ERROR "E"



namespace Log {

	File logfile;


	void init() noexcept {

#ifdef ARC_LOG_STDIO_UNSYNC

		try {
			std::ios_base::sync_with_stdio(false);
		} catch (const std::exception&) {
	#if defined(ARC_LOG_EXCEPTION_ABORT) && !defined(ARC_FINAL_BUILD)
			arc_abort();
	#endif
		}

#endif

	}


	void openLogFile(Path path) noexcept {

		try {

			bool dirCreated = Directory(path).create();

			if (!dirCreated) {
				Log::error("Log", "Failed to create log file directory '%s'", path.toString().c_str());
				return;
			}

			path.append("log_" + Time::getTimestamp() + ".txt");
			logfile.open(path, File::Out);

			if (!logfile.isOpen()) {
				Log::error("Log", "Failed to open log file '%s'", path.toString().c_str());
			}

		} catch (const std::exception&) {
#if defined(ARC_LOG_EXCEPTION_ABORT) && !defined(ARC_FINAL_BUILD)
			arc_abort();
#endif
		}

	}


	void closeLogFile() noexcept {

		try {
			logfile.close();
		} catch (const std::exception&) {
#if defined(ARC_LOG_EXCEPTION_ABORT) && !defined(ARC_FINAL_BUILD)
			arc_abort();
#endif
		}

	}


	void print(const std::string& level, const std::string& subsystem, const std::string& message) noexcept {

		try {

			std::string line = "[" + level + ": " + subsystem + "] " + message;
			std::cout << line << std::endl;

			if (logfile.isOpen()) {
				logfile.writeLine(line);
			}

		} catch (const std::exception&) {
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