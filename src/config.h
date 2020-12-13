#pragma once

#include <string>



namespace Config {

	//Returns the application name
	const std::string& getApplicationName();

	//Returns the Uri root path
	const std::string& getUriRootPath();

	//Returns the Uri asset path
	const std::string& getUriAssetPath();

	//Returns the Uri log path
	const std::string& getUriLogPath();

	//Returns the Uri screenshot path
	const std::string& getUriScreenshotPath();

}