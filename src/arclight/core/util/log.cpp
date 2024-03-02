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
#include "common/config.hpp"

#include <iostream>


namespace Log {

	File logFile;


	void init() noexcept {

#ifdef ARC_LOG_STDIO_UNSYNC

		try {
			std::ios_base::sync_with_stdio(false);
		} catch (const std::exception &) {
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
				LogE("Log").print("Failed to create log file directory '%s'", path.toString().c_str());
				return;
			}

			path.append("log_" + Time::getTimestamp() + ".txt");

			if (!logFile.open(path, File::Out)) {
				LogE("Log").print("Failed to open log file '%s'", path.toString().c_str());
			}

		} catch (const std::exception &) {
#if defined(ARC_LOG_EXCEPTION_ABORT) && !defined(ARC_FINAL_BUILD)
			arc_abort();
#endif
		}

	}

	void closeLogFile() noexcept {

		try {
			logFile.close();
		} catch (const std::exception &) {
#if defined(ARC_LOG_EXCEPTION_ABORT) && !defined(ARC_FINAL_BUILD)
			arc_abort();
#endif
		}

	}

}


void RawLog::flush() noexcept {

	try {

		std::string str = buffer.str();
		buffer.str("");

		std::cout << str;

		if (!str.empty() && str.back() == '\n') {
			str.pop_back();
		}

		if (Log::logFile.isOpen()) {
			Log::logFile.writeLine(str);
		}

	} catch(const std::exception& e) {
		// There's literally nothing we can do here
#if defined(ARC_LOG_EXCEPTION_ABORT) && !defined(ARC_FINAL_BUILD)
		arc_abort();
#endif
	}

}


RawLog& RawLog::operator<<(Token token) {

	switch(token) {
		case Token::Endl:
			buffer << '\n';
			flush();
			break;
		case Token::Dec:
			buffer << std::dec;
			break;
		case Token::Hex:
			buffer << std::hex;
			break;
		case Token::Upper:
			buffer << std::uppercase;
			break;
		case Token::NoUpper:
			buffer << std::nouppercase;
			break;
		case Token::BoolAlpha:
			buffer << std::boolalpha;
			break;
		case Token::NoBoolAlpha:
			buffer << std::noboolalpha;
			break;
		case Token::Forward:
			reversed = false;
			break;
		case Token::Reversed:
			reversed = true;
			break;
		case Token::Flush:
			flush();
			break;
		default:
			arc_force_assert("Invalid Log token");
			break;
	}

	return *this;

}

RawLog& RawLog::operator<<(TabToken tab) {

	const std::string& str = buffer.str();

	SizeT pos = str.find_last_of('\n');

	if (pos == std::string::npos) {
		pos = 0;
	} else {
		pos++;
	}

	SizeT align = (str.length() - pos) % tab.width;

	for (SizeT i = 0; i < align; i++) {
		buffer << tab.c;
	}

	return *this;

}

RawLog& RawLog::operator<<(const std::type_info& info) {

	buffer << info.name();

	return *this;

}

RawLog& RawLog::operator<<(const std::exception& e) {

	buffer << Exception::getMessage(e);
	buffer << Exception::getStackTrace(e);
	return *this;

}