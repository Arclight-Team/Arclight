/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 os.hpp
 */

#pragma once

#include "process.hpp"

#include <string>
#include <optional>


namespace OS {

	namespace Environment {

		std::optional<std::string> getVariable(const std::string& var);
		bool setVariable(const std::string& var, const std::string& contents);

	}

	bool logout();
	bool restart(bool force = false);
	bool shutdown(bool force = false);

	bool addToStartup(const std::string& name, const Path& path = Process::getCurrentProcess().getExecutablePath(), bool allUsers = false);
	bool removeFromStartup(const std::string& name, bool allUsers = false);

	enum class LaunchAction {
		Default,
		Edit,
		Explore,
		Find,
		Open,
		Print,
		Properties,
		RunAsAdmin
	};

	bool launch(LaunchAction action, const Path& path, const std::string& params = "", bool switchCWD = false);

}