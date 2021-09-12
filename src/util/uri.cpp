#include "uri.h"
#include "assert.h"
#include "config.h"
#include "log.h"

#include <vector>


Uri::Uri() : path() {}


Uri::Uri(const std::string& path) {
	setPath(path);
};




void Uri::move(const std::string& path) {
	this->path /= path;
}



void Uri::setPath(const std::string& path) {

	if(path.empty()) {
		return;
	}

	if(path.size() >= 2 && path[1] == '/') {
		this->path = getSpecialUriRootPath(path[0]) + path;
	} else {
		this->path = path;
	}

}



bool Uri::createDirectory() const {

	if (directoryExists()) {
		return true;
	}

	bool created = false;

	try {
		created = std::filesystem::create_directories(path);
	} catch (std::exception&) {
		Log::error("Uri", "Failed to create directory '%s'", path.string().c_str());
		return false;
	}

	if (!created) {
		Log::error("Uri", "Failed to create directory '%s'", path.string().c_str());
		return false;
	}

	return true;

}



bool Uri::copy() const {
	
}



bool Uri::remove() const {

}



bool Uri::fileExists() const {
	return Uri::fileExists(path.string());
}



bool Uri::directoryExists() const {
	return Uri::directoryExists(path.string());
}



std::string Uri::getPath() const {
	return path.string();
}



bool Uri::fileExists(const std::string& path) {
	return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}



bool Uri::directoryExists(const std::string& path) {
	return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}


	
void Uri::setSpecialUriHandler(char symbol, SpecialUriHandler handler) {

	if(std::string(&symbol).find_first_not_of("/<>:/\\|?*") == std::string::npos) {

		Log::warn("Uri", "Cannot set uri handler for special symbol %c", symbol);
		return;

	}

	specialHandlers[symbol] = handler;

}


std::string Uri::getSpecialUriRootPath(char symbol) {

	if(specialHandlers.contains(symbol)) {
		return specialHandlers[symbol]();
	}

	return "";

}



std::string Uri::getSystemTempPath() {
	return std::filesystem::temp_directory_path().string();
}