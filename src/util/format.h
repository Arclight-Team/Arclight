#pragma once

#include <string>
//#include <format>

#include "config.h"


namespace Util {


	template<class... Args>
	std::string format(std::string message, Args&&... args) {
		
		static const char* sequenceList[] = {
			"appname"
		};

		static const char*(*sequenceFunctions[])() = {
			&Config::getApplicationName
		};


		std::size_t openBracket = 0;
		std::size_t closeBracket = 0;

		while (openBracket = message.find_first_of('{', openBracket), openBracket != std::string::npos) {

			closeBracket = message.find_first_of('}', openBracket);

			if (closeBracket == std::string::npos) {
				break;
			}
				
			bool found = false;
			const std::string& sequence = message.substr(openBracket + 1, closeBracket - openBracket - 1);

			for (unsigned i = 0; i < 1; i++) {

				if (sequence == sequenceList[i]) {

					const std::string& formattedText = sequenceFunctions[i]();
					message = message.erase(openBracket, closeBracket - openBracket + 1);
					message.insert(openBracket, formattedText);
					openBracket += formattedText.size();
					found = true;

					break;

				}

			}

			if (!found) {
				openBracket = closeBracket + 1;
			}

		}

		//return std::format(message, args...);
		
		char buffer[256];
		std::snprintf(buffer, 256, message.c_str(), args...);

		return std::string(buffer);
	}

}