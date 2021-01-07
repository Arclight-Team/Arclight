#pragma once

#include "format.h"
#include "util/format.h"
#include <string>


namespace Log {


	void init();
	void openLogFile();
	void closeLogFile();

	namespace Raw {

		void debug(const std::string& subsystem, const std::string& message);
		void info(const std::string& subsystem, const std::string& message);
		void warn(const std::string& subsystem, const std::string& message);
		void error(const std::string& subsystem, const std::string& message);

	}


	template<class... Args>
	void debug(const std::string& subsystem, const std::string& message, Args&&... args) {
#ifndef NDEBUG
		Raw::debug(subsystem, Util::format(message, std::forward<Args>(args)...));
#endif
	}

	template<class... Args>
	void info(const std::string& subsystem, const std::string& message, Args&&... args) {
		Raw::info(subsystem, Util::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void warn(const std::string& subsystem, const std::string& message, Args&&... args) {
		Raw::warn(subsystem, Util::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void error(const std::string& subsystem, const std::string& message, Args&&... args) {
		Raw::error(subsystem, Util::format(message, std::forward<Args>(args)...));
	}

}