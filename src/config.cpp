#include "config.h"



namespace Config {

	const char* appName = "Project Arclight";
	const char* uriRoot = "../../assets";
	const char* logDirName = "../../log";
	const char* logFileName = "appname-%version-%date-%u32.txt";


	const char* getApplicationName() {
		return appName;
	}

	const char* getURIRootPath() {
		return uriRoot;
	}

	const char* getLogDirectoryName() {
		return logDirName;
	}

	const char* getLogFileName() {
		return logFileName;
	}

}