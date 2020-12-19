#pragma once

#include <string>
//#include <format>

#include "config.h"


namespace Util {


	template<class... Args>
	std::string format(std::string message, Args&&... args) {

		//return std::format(message, args...);
		
		char buffer[256];
		std::snprintf(buffer, 256, message.c_str(), args...);

		return std::string(buffer);

	}

}