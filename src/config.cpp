#include "config.h"



namespace Config {

	const std::string appName = "Project Arclight";
	const std::string baseWindowTitle = "Arclight Engine Debug (ATR, x64)";
	const std::string uriRoot = "../";
	const std::string uriAsset = "assets/";
	const std::string uriLog = "log/";
	const std::string uriScreenshot = "screenshots/";

	const u32 defaultWindowWidth = 400;
	const u32 defaultWindowHeight = 400;


	const std::string& getApplicationName() {
		return appName;
	}

	const std::string& getBaseWindowTitle() {
		return baseWindowTitle;
	}

	u32 getDefaultWindowWidth() {
		return defaultWindowWidth;
	}

	u32 getDefaultWindowHeight() {
		return defaultWindowHeight;
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