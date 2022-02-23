/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 log.hpp
 */

#pragma once

#include "filesystem/path.hpp"
#include "string.hpp"



namespace Log {


	void init() noexcept;
	void openLogFile(Path path) noexcept;
	void closeLogFile() noexcept;


	//Direct logging
	void debug(const std::string& subsystem, const std::string& message) noexcept;
	void info(const std::string& subsystem, const std::string& message) noexcept;
	void warn(const std::string& subsystem, const std::string& message) noexcept;
	void error(const std::string& subsystem, const std::string& message) noexcept;


	//Formatted logging
	template<class... Args>
	void debug(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		debug(subsystem, String::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void info(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		info(subsystem, String::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void warn(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		warn(subsystem, String::format(message, std::forward<Args>(args)...));
	}

	template<class... Args>
	void error(const std::string& subsystem, const std::string& message, Args&&... args) noexcept {
		error(subsystem, String::format(message, std::forward<Args>(args)...));
	}

}