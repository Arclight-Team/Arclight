#include "util/file.h"
#include "util/log.h"
#include "util/assert.h"
#include "config.h"

#include <vector>


Uri::Uri() : path() {}


Uri::Uri(const char* path) {
	setPath(path);
};


Uri::Uri(const std::string& path) {
	setPath(path);
};



void Uri::setPath(const std::string& path) {

	if (!path.empty() && path[0] == ':') {
		this->path = Config::getURIRootPath() + path.substr(1);
	} else {
		this->path = path;
	}

}



bool Uri::createDirectory() {

	if (validDirectory()) {
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



bool Uri::validFile() const {
	return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
}



bool Uri::validDirectory() const {
	return std::filesystem::exists(path) && std::filesystem::is_directory(path);
}



std::string Uri::getPath() const {
	return path.string();
}




File::File() : openFlags(0) {}


File::File(const Uri& path, File::Flags flags) : filepath(path), openFlags(flags) {};



bool File::open(const Uri& path, File::Flags flags) {

	arc_assert((flags & File::In) || (flags & File::Out), "Invalid file flags requested: %02X", flags);

	if (isOpen()) {
		Log::warn("File", "Attempting to open stream that has already been opened. Open: '%s', requested '%s'", filepath.getPath().c_str(), path.getPath().c_str());
		return false;
	}

	filepath = path;
	openFlags = flags;

	stream.open(path.getPath(), flags);

	return isOpen();

}



void File::close() {

	if (!isOpen()) {
		Log::warn("File", "Attempting to close stream that is already closed (URI = '%s')", filepath.getPath().c_str());
		return;
	}

	stream.close();

}



std::string File::read(u64 count) {

	arc_assert(isOpen(), "Attempted to read from an unopened file");
	arc_assert(openFlags & File::In, "Attempted to read from an output stream");
	arc_assert(!(openFlags & File::Binary), "Attempted to read text from a binary stream");

	std::string text;
	text.resize(count);
	stream.read(text.data(), count);

	return text;

}



std::string File::readWord() {

	arc_assert(isOpen(), "Attempted to read from an unopened file");
	arc_assert(openFlags & File::In, "Attempted to read from an output stream");
	arc_assert(!(openFlags & File::Binary), "Attempted to read text from a binary stream");

	std::string word;
	stream >> word;

	return word;

}



std::string File::readLine() {

	arc_assert(isOpen(), "Attempted to read from an unopened file");
	arc_assert(openFlags & File::In, "Attempted to read from an output stream");
	arc_assert(!(openFlags & File::Binary), "Attempted to read text from a binary stream");

	std::string line;
	std::getline(stream, line);

	return line;

}



std::string File::readAll() {

	arc_assert(isOpen(), "Attempted to read from an unopened file");
	arc_assert(openFlags & File::In, "Attempted to read from an output stream");
	arc_assert(!(openFlags & File::Binary), "Attempted to read text from a binary stream");

	std::vector<char> bytes(getFileSize());
	stream.read(bytes.data(), bytes.size());

	return std::string(bytes.data(), bytes.size());
}


void File::write(const std::string& text) {

	arc_assert(isOpen(), "Attempted to write to an unopened file");
	arc_assert(openFlags & File::Out, "Attempted to write to an input stream");
	arc_assert(!(openFlags & File::Binary), "Attempted to write text to a binary stream");

	stream << text;

}



void File::writeLine(const std::string& line) {

	arc_assert(isOpen(), "Attempted to write to an unopened file");
	arc_assert(openFlags & File::Out, "Attempted to write to an input stream");
	arc_assert(!(openFlags & File::Binary), "Attempted to write text to a binary stream");

	stream << line << '\n';

}



void File::read(u8* data, u64 count) {

	arc_assert(isOpen(), "Attempted to read from an unopened file");
	arc_assert(openFlags & File::In, "Attempted to read from an output stream");
	arc_assert(openFlags & File::Binary, "Attempted to read bytes from a text-based stream");

	stream.read(reinterpret_cast<char*>(data), count);

}


void File::write(u8* data, u64 count) {

	arc_assert(isOpen(), "Attempted to write to an unopened file");
	arc_assert(openFlags & File::Out, "Attempted to write to an input stream");
	arc_assert(openFlags & File::Binary, "Attempted to write bytes to a text-based stream");

	stream.write(reinterpret_cast<const char*>(data), count);

}



void File::seek(u64 pos) {
	arc_assert(isOpen(), "Attempted to seek in an unopened file");
	stream.seekg(pos, std::ios::beg);
}



void File::seekRelative(i64 pos) {
	arc_assert(isOpen(), "Attempted to seek in an unopened file");
	stream.seekg(pos, std::ios::cur);
}



bool File::isOpen() const {
	return stream.is_open();
}


u64 File::getFileSize() const {
	arc_assert(filepath.validFile(), "Invalid URI '%s'", filepath.getPath().c_str());
	return std::filesystem::file_size(filepath.getPath());
}



Uri File::getUri() const {
	return filepath;
}



File::Flags File::getStreamFlags() const {
	return openFlags;
}



u64 File::getLastWriteTime() const {
	arc_assert(filepath.validFile(), "Invalid URI '%s'", filepath.getPath().c_str());
	return std::filesystem::last_write_time(filepath.getPath()).time_since_epoch().count();
}