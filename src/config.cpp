#include "config.h"



namespace Config {

	const std::string appName = "Project Arclight";
	const std::string uriRoot = "../../";
	const std::string uriAsset = "assets/";
	const std::string uriLog = "log/";
	const std::string uriScreenshot = "screenshots/";


	const std::string& getApplicationName() {
		return appName;
	}

	const std::string& getUriRootPath() {
		return uriRoot;
	}

	const std::string& getUriAssetPath() {
		return uriAsset;
	}

	const std::string& getUriLogPath() {
		return uriLog;
	}

	const std::string& getUriScreenshotPath() {
		return uriScreenshot;
	}

}