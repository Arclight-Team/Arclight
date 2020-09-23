#pragma once

#include <string>


namespace Log {

	/*
		Opens the log file given by the config
	*/
	void openLogFile();

	/*
		Closes the log file given by the config
	*/
	void closeLogFile();

	/*
		Prints a debug string
	*/
	void debug(const std::string& subsystem, const std::string& message);

	/*
		Prints an info string
	*/
	void info(const std::string& subsystem, const std::string& message);

	/*
		Prints a warning string
	*/
	void warn(const std::string& subsystem, const std::string& message);

	/*
		Prints an error string
	*/
	void error(const std::string& subsystem, const std::string& message);

}