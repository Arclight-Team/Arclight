#pragma once

#include "format.h"
#include "util/format.h"
#include <string>


namespace Log {


	void init();
	void openLogFile();
	void closeLogFile();

	namespace Raw {

		void debug(const std::string& subsystem, const std::string& message) noexcept;
		void info(const std::string& subsystem, const std::string& message) noexcept;
		void warn(const std::string& subsystem, const std::string& message) noexcept;
		void error(const std::string& subsystem, const std::string& message) noexcept;

	}


	template<class... Args>
	void debug(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		Raw::debug(subsystem, Util::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void info(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		Raw::info(subsystem, Util::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void warn(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		Raw::warn(subsystem, Util::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void error(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		Raw::error(subsystem, Util::format(message, std::forward<Args>(args)...));
	}

}