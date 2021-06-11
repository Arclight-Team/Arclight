#include "util/log.h"
#include "util/file.h"
#include "util/time.h"
#include "util/assert.h"
#include "config.h"
#include "arcconfig.h"

#include <iostream>
#include <sstream>


#define LOG_DEBUG "D"
#define LOG_INFO  "I"
#define LOG_WARN  "W"
#define LOG_ERROR "E"



namespace Log {

	File logfile;


	void init() {
#ifdef ARC_LOG_STDIO_UNSYNC
		std::ios_base::sync_with_stdio(false);
#endif
	}


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

		void print(const std::string& level, const std::string& subsystem, const std::string& message) noexcept {

			try {

				std::string line = "[" + level + ": " + subsystem + "] " + message;
				std::cout << line << std::endl;

				if (logfile.isOpen()) {
					logfile.writeLine(line);
				}

			} catch (std::exception&) {
				//There's literally nothing we can do here
#ifdef ARC_LOG_EXCEPTION_ABORT
				arc_abort();
#endif
			}

		}

		void debug(const std::string& subsystem, const std::string& message) noexcept {
			print(LOG_DEBUG, subsystem, message);
		}


		void info(const std::string& subsystem, const std::string& message) noexcept {
			print(LOG_INFO, subsystem, message);
		}


		void warn(const std::string& subsystem, const std::string& message) noexcept {
			print(LOG_WARN, subsystem, message);
		}


		void error(const std::string& subsystem, const std::string& message) noexcept {
			print(LOG_ERROR, subsystem, message);
		}

	}

}