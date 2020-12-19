#include "util/log.h"
#include "util/file.h"
#include "util/time.h"
#include "config.h"

#include <iostream>
#include <sstream>


#define LOG_DEBUG "D"
#define LOG_INFO  "I"
#define LOG_WARN  "W"
#define LOG_ERROR "E"



namespace Log {

	File logfile;


	void openLogFile() {

		Uri logfileUri(Config::getUriLogPath());
		bool dirCreated = logfileUri.createDirectory();

		if (!dirCreated) {
			Log::error("Log", "Failed to create log file directory '%s'", logfileUri.getPath().c_str());
			return;
		}

		logfileUri.move("log_" + Time::getTimestamp() + ".txt");
		logfile.open(logfileUri, File::Out);

		if (!logfile.isOpen()) {
			Log::error("Logger", "Failed to open log file '%s'", logfileUri.getPath().c_str());
		}

	}


	void closeLogFile() {
		logfile.close();
	}


	namespace Raw {

		void print(const std::string& level, const std::string& subsystem, const std::string& message) {

			std::stringstream ss;
			ss << "[" << level << "] <" << subsystem << "> " << message;
			std::cout << ss.str() << std::endl;

			if (logfile.isOpen()) {
				logfile.writeLine(ss.str());
			}

		}

		void debug(const std::string& subsystem, const std::string& message) {
			print(LOG_DEBUG, subsystem, message);
		}


		void info(const std::string& subsystem, const std::string& message) {
			print(LOG_INFO, subsystem, message);
		}


		void warn(const std::string& subsystem, const std::string& message) {
			print(LOG_WARN, subsystem, message);
		}


		void error(const std::string& subsystem, const std::string& message) {
			print(LOG_ERROR, subsystem, message);
		}

	}

}