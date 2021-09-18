#include "uri.h"
#include "util/assert.h"
#include "config.h"
#include "log.h"

#include <vector>


Uri::Uri() : path() {}


Uri::Uri(const char* path, bool skipParsing) {
	setPath(std::string(path), skipParsing);
}


Uri::Uri(const std::string& path, bool skipParsing) {
	setPath(path, skipParsing);
};




void Uri::move(const std::string& path) {
	this->path /= path;
}



void Uri::setPath(const std::string& path, bool skipParsing) {

	if(path.empty()) {
		return;
	}

	if(!skipParsing && path.size() >= 2 && path[1] == '/') {
		this->path = getSpecialUriRootPath(path[0]) + path.substr(2);
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



bool Uri::copy(const Uri& target, u32 options) const {

	std::filesystem::copy_options opt = std::filesystem::copy_options::none;

	if(options & CopyOptions::CopySymlinks) {
		opt |= std::filesystem::copy_options::copy_symlinks;
	}

	if(options & CopyOptions::CreateHardlinks) {
		opt |= std::filesystem::copy_options::create_hard_links;
	}

	if(options & CopyOptions::CreateSymlinks) {
		opt |= std::filesystem::copy_options::create_symlinks;
	}

	if(options & CopyOptions::DirectoriesOnly) {
		opt |= std::filesystem::copy_options::directories_only;
	}

	if(options & CopyOptions::OverwriteExisting) {
		opt |= std::filesystem::copy_options::overwrite_existing;
	}

	if(options & CopyOptions::Recursive) {
		opt |= std::filesystem::copy_options::recursive;
	}

	if(options & CopyOptions::SkipExisting) {
		opt |= std::filesystem::copy_options::skip_existing;
	}

	if(options & CopyOptions::SkipSymlinks) {
		opt |= std::filesystem::copy_options::skip_symlinks;
	}

	if(options & CopyOptions::UpdateExisting) {
		opt |= std::filesystem::copy_options::update_existing;
	}

	try {

		std::filesystem::copy(path, target.getPath(), opt);

	} catch(std::exception&) {

		Log::error("Uri", "Failed to copy contents from %s to %s", path.string().c_str(), target.getPath().c_str());
		return false;

	}

	return true;

}



bool Uri::remove() const {

	bool removed = false;

	try {

		removed = std::filesystem::remove(path);

	} catch (std::exception&) {

		Log::error("Uri", "Failed to remove '%s'", path.string().c_str());
		return false;

	}

	if (!removed) {

		Log::error("Uri", "Failed to remove '%s'", path.string().c_str());
		return false;
		
	}

	return true;

}



u64 Uri::removeRecursively() const {

	u64 removedEntities = 0;

	try {

		removedEntities = std::filesystem::remove_all(path);

	} catch (std::exception&) {

		Log::error("Uri", "Failed to remove '%s' recursively", path.string().c_str());

	}

	return removedEntities;

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



Uri Uri::rootName() const {
	return Uri(path.root_name().string(), true);
}



Uri Uri::rootDirectory() const {
	return Uri(path.root_directory().string(), true);
}



Uri Uri::rootPath() const {
	return Uri(path.root_path().string(), true);
}



Uri Uri::relativePath() const {
	return Uri(path.relative_path().string(), true);
}



Uri Uri::parentPath() const {
	return Uri(path.parent_path().string(), true);
}



Uri Uri::filename() const {
	return Uri(path.filename().string(), true);
}



Uri Uri::stem() const {
	return Uri(path.stem().string(), true);
}



Uri Uri::extension() const {
	return Uri(path.extension().string(), true);
}



bool Uri::empty() const {
	return path.empty();
}



bool Uri::hasRootName() const {
	return path.has_root_name();
}



bool Uri::hasRootDirectory() const {
	return path.has_root_directory();
}



bool Uri::hasRootPath() const {
	return path.has_root_path();
}



bool Uri::hasRelativePath() const {
	return path.has_relative_path();
}



bool Uri::hasParentPath() const {
	return path.has_parent_path();
}



bool Uri::hasFilename() const {
	return path.has_filename();
}



bool Uri::hasStem() const {
	return path.has_stem();
}



bool Uri::hasExtension() const {
	return path.has_extension();
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