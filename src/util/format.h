#pragma once

#include <string>

#include "config.h"


namespace Util {


	template<class... Args>
	std::string format(std::string message, Args&&... args) noexcept {

		try {
			std::string out;
			out.resize(message.length() * 2);

			std::snprintf(out.data(), out.size(), message.c_str(), args...);

			return out;
		}
		catch (std::exception&) {
			//We cannot log here, so return an empty string
			return "";
		}

	}

	template<class... Args>
	std::wstring format(std::wstring message, Args&&... args) noexcept {

		try {
			std::wstring out;
			out.resize(message.length() * 2);

			std::swprintf(out.data(), out.size(), message.c_str(), args...);

			return out;
		}
		catch (std::exception&) {
			//We cannot log here, so return an empty string
			return "";
		}

	}

}