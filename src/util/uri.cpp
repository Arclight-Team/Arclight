#include "uri.h"
#include "assert.h"
#include "config.h"
#include "log.h"

#include <vector>


Uri::Uri() : path() {}


Uri::Uri(const char* path) {
	setPath(path);
};


Uri::Uri(const std::string& path) {
	setPath(path);
};



void Uri::setPath(const std::string& path) {

	if (!path.empty() && path.starts_with(":/")) {
		this->path = Config::getUriAssetPath() + path.substr(2);
	} else {
		this->path = path;
	}

}



bool Uri::createDirectory() {

	if (directoryExists()) {
		return true;
	}

	bool created = false;

	try {
		created = std::filesystem::create_directories(path);
	} catch (std::exception&) {
		Log::error("Logger", "Failed to create directory '%s'", path.string().c_str());
		return false;
	}

	if (!created) {
		Log::error("Logger", "Failed to create directory '%s'", path.string().c_str());
		return false;
	}

	return true;

}



void Uri::move(const std::string& path) {
	this->path /= path;
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



void Uri::setApplicationUriRoot(const std::string& root) {
	std::filesystem::current_path(root);
}