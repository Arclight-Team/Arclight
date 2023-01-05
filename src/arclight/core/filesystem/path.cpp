/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 path.cpp
 */

#include "path.hpp"

#include <utility>
#include "util/log.hpp"



Path::Path() : path("") {}

Path::Path(const char* path, bool annotated) : Path(std::string(path), true) {}

Path::Path(const std::string& path, bool annotated) : path(annotated ? convertAnnotatedPath(path) : path) {}

Path::Path(std::filesystem::path path) : path(std::move(path)) {}



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



std::string Path::toString() const {
	return path.generic_string();
}



std::string Path::toNativeString() const {
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

	path = std::move(getAbsolute().path);
	return *this;

}



Path& Path::toCanonical() {

	path = std::move(getCanonical().path);
	return *this;

}



Path& Path::toRelative() {

	path = std::move(getRelative().path);
	return *this;

}



Path Path::getAbsolute() const {
	return Path(std::filesystem::absolute(path));
}



Path Path::getCanonical() const {
	return Path(std::filesystem::canonical(path));
}



Path Path::getRelative() const {
	return Path(std::filesystem::relative(path));
}



std::string Path::relativeAgainst(const Path& base) const {
	return std::filesystem::relative(path, base.path).string();
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
	return toString();
}



const std::filesystem::path& Path::getHandle() const {
	return path;
}



Path Path::getCurrentWorkingDirectory() {
	return Path(std::filesystem::current_path());
}



void Path::setCurrentWorkingDirectory(const Path& path) {
	std::filesystem::current_path(path.path);
}



std::string Path::convertAnnotatedPath(const std::string& annotatedPath) {

	if(annotatedPath.size() >= 2 && annotatedPath[0] != '.' && annotatedPath[1] == '/') {
		return getAnnotatedPathPrefix(annotatedPath[0]) + annotatedPath.substr(2);
	}

	return annotatedPath;

}



std::string Path::getAnnotatedPathPrefix(char annotation) {

	if(annotatedPrefixes.contains(annotation)) {
		return annotatedPrefixes[annotation];
	}

	LogW("Path").print("Illegally annotated path [%c]", annotation);

	return "";
	
}



void Path::setAnnotatedPathPrefix(char annotation, const std::string& prefix) {

	if (std::string("./<>:/\\|?*").find(annotation) != std::string::npos) {
		
		LogW("Path").print("Cannot set annotated path prefix for annotation %c", annotation);
		return;

	} else {

		annotatedPrefixes[annotation] = prefix;

	}

}



Path operator+(const Path& a, const Path& b) {
	return Path(a) += b;
}



Path operator/(const Path& a, const Path& b) {
	return Path(a) /= b;
}



RawLog& operator<<(RawLog& log, const Path& path) {
	return log << path.toString();
}
