/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 os.hpp
 */

#pragma once

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

}