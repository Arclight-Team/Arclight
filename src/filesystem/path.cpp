#include "path.h"
#include "util/log.h"



Path::Path() : path("") {}

Path::Path(const std::string& path) : path(convertAnnotatedPath(path)) {}

Path::Path(const std::filesystem::path& path) : path(path) {}



Path& Path::set(Path&& path) {

    this->path = std::move(path.path);
    return *this;

}



Path& Path::set(const Path& path) {

    this->path = path.path;
    return *this;

}



Path& Path::append(const std::string& str) {
    
    path.append(str);
    return *this;

}



Path& Path::concat(const std::string& str) {

    path.concat(str);
    return *this;

}



Path& Path::setStem(const std::string& stem) {
    return setFilename(stem + getExtension());
}



Path& Path::setExtension(const std::string& ext) {

    path.replace_extension(ext);
    return *this;

}



Path& Path::setFilename(const std::string& name) {

    path.replace_filename(name);
    return *this;

}



std::string Path::getPath() const {
    return path.string();
}



std::string Path::getPreferredPath() const {
    return std::filesystem::path(path).make_preferred().string();
}



std::string Path::getStem() const {
    return path.stem().string();
}



std::string Path::getExtension() const {
    return path.extension().string();
}



std::string Path::getFilename() const {
    return path.filename().string();
}



bool Path::hasStem() const {
    return path.has_stem();
}



bool Path::hasExtension() const {
    return path.has_extension();
}



bool Path::hasFilename() const {
    return path.has_filename();
}



Path& Path::toAbsolute() {

    try {
        path = std::filesystem::absolute(path);
    } catch(std::exception& e) {
        Log::error("Path", "Failed to convert path \"%s\" to absolute form: %s", path.string().c_str(), e.what());
    }

    return *this;

}



Path& Path::toCanonical() {

    path = std::filesystem::weakly_canonical(path);
    return *this;

}



Path& Path::toRelative() {

    try {
        path = std::filesystem::relative(path);
    } catch(std::exception& e) {
        Log::error("Path", "Failed to convert path \"%s\" to relative form: %s", path.string().c_str(), e.what());
    }
    
    return *this;

}



std::string Path::relativeAgainst(const Path& base) const {

    try {
        return std::filesystem::relative(path, base.path).string();
    } catch(std::exception& e) {
        Log::error("Path", "Failed to convert path \"%s\" to relative base form against \"%s\": %s", path.string().c_str(), base.path.string().c_str(), e.what());
    }

    return "";

}



bool Path::isAbsolute() const {
    return path.is_absolute();
}



bool Path::isRelative() const {
    return path.is_relative();
}



void Path::clear() {
    path.clear();
}



bool Path::isEmpty() const {
    return path.empty();
}



bool Path::exists() const {
    return std::filesystem::exists(path);
}



bool Path::equivalent(const Path& other) const {

    std::error_code ec;
    return std::filesystem::equivalent(path, other.path, ec);

}



Path Path::root() const {
    return Path(std::filesystem::canonical(path).root_path());
}



Path Path::parent() const {
    return Path(std::filesystem::canonical(path).parent_path());
}



Path::operator std::string() const {
    return getPath();
}



Path Path::getCurrentWorkingDirectory() {
    return Path(std::filesystem::current_path());
}



void Path::setCurrentWorkingDirectory(const Path& path) {
    std::filesystem::current_path(path.path);
}



std::string Path::convertAnnotatedPath(const std::string& annotatedPath) {

    if(annotatedPath.size() >= 2 && annotatedPath[1] == '/') {
		return getAnnotatedPathPrefix(annotatedPath[0]) + annotatedPath.substr(2);
	}

    return annotatedPath;

}



std::string Path::getAnnotatedPathPrefix(char annotation) {

    if(annotatedPrefixes.contains(annotation)) {
        return annotatedPrefixes[annotation];
    }

    Log::warn("Path", "Illegally annotated path [%c]", annotation);

    return "";
    
}



void Path::setAnnotatedPathPrefix(char annotation, const std::string& prefix) {

    if (std::string("/<>:/\\|?*").find(annotation) != std::string::npos) {
        
        Log::warn("Path", "Cannot set annotated path prefix for annotation %c", annotation);
		return;

    } else {

        annotatedPrefixes[annotation] = prefix;

    }

}