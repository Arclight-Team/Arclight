#pragma once
#include "format.h"
#include <string>
#include "util/format.h"


namespace Log {


	void openLogFile();

	void closeLogFile();

	namespace Raw {

		void debug(const std::string& subsystem, const std::string& message);
		void info(const std::string& subsystem, const std::string& message);
		void warn(const std::string& subsystem, const std::string& message);
		void error(const std::string& subsystem, const std::string& message);

	}


	template<class... Args>
	void debug(const std::string& subsystem, const std::string& message, const Args&... args) {
#ifndef NDEBUG
		Raw::debug(subsystem, Util::format(message, args...));
#endif
	}

	template<class... Args>
	void info(const std::string& subsystem, const std::string& message, const Args&... args) {
		Raw::info(subsystem, Util::format(message, args...));
	}

	template<class... Args>
	void warn(const std::string& subsystem, const std::string& message, const Args&... args) {
		Raw::warn(subsystem, Util::format(message, args...));
	}

	template<class... Args>
	void error(const std::string& subsystem, const std::string& message, const Args&... args) {
		Raw::error(subsystem, Util::format(message, args...));
	}

}