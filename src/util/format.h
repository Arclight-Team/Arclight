#pragma once

#include <string>
//#include <format>

#include "config.h"


namespace Util {


	template<class... Args>
	std::string format(std::string message, const Args&... args) {
		/*
		static const char* sequenceList[] = {
			"appname"
		};

		static const char*(*)() sequenceFunctions[] = {
			&Config::getApplicationName()
		};


		unsigned openBracket = 0;
		unsigned closeBracket = 0;

		while (openBracket = message.find_first_of('{', openBracket), openBracket != std::string::npos) {

			closeBracket = message.find_first_of('}', openBracket);

			if (closeBracket == std::string::npos) {
				break;
			}
				
			const std::string& sequence = message.substr(openBracket, closeBracket - openBracket);

			for (unsigned i = 0; i < 1; i++) {

				if (sequence == sequenceList[i]) {

					const std::string& formattedText = sequenceFunctions[i](sequence);
					message = message.erase(openBracket, closeBracket - openBracket);
					message.insert(openBracket, formattedText);
					openBracket += formattedText.size();

					break;

				}

			}

		}

		return std::format(message, args...);
		*/
		return "";
	}

}