#pragma once

#include <string>

#include "config.h"


namespace Util {


	template<class... Args>
	std::string format(std::string message, Args&&... args) noexcept {

		
		char buffer[256];
		std::snprintf(buffer, 256, message.c_str(), args...);

		try {
			return std::string(buffer);
		} catch (std::exception&) {
			//We cannot log here, so return an empty string
			return "";
		}

	}

}