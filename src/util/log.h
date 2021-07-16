#pragma once

#include "string.h"



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
		Raw::debug(subsystem, String::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void info(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		Raw::info(subsystem, String::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void warn(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		Raw::warn(subsystem, String::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void error(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		Raw::error(subsystem, String::format(message, std::forward<Args>(args)...));
	}

}