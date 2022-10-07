/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 file.cpp
 */

#include "file.hpp"
#include "util/assert.hpp"
#include "util/log.hpp"



constexpr static std::ios::openmode convertFlagsToStdFlags(u32 flags) {

	//Safe because mode gets or'ed later
	std::ios::openmode mode = static_cast<std::ios::openmode>(0);

	if(!flags) {
		LogW("File") << "File flags cannot be 0, resorting to File::In";
		return std::ios::in;
	}

	if(flags & File::In) { mode |= std::ios::in; }
	if(flags & File::Out) { mode |= std::ios::out; }
	if(!(flags & File::Text)) { mode |= std::ios::binary; }
	if(flags & File::AtEnd) { mode |= std::ios::ate; }
	if(flags & File::Append) { mode |= std::ios::app; }
	if(flags & File::Trunc) { mode |= std::ios::trunc; }

	return mode;

}




File::File() : openFlags(0) {}

File::File(const Path& path, u32 flags) : filePath(path), openFlags(flags) { open(path, flags); };



bool File::open(const Path& path, u32 flags) {

	filePath = path;
	openFlags = flags;

	if (!flags) {
		return false;
	}

	stream.open(path.toString(), convertFlagsToStdFlags(flags));

	return isOpen();

}



void File::close() {
	stream.close();
}



std::string File::readChars(u64 count) {

	std::string text;
	text.resize(count);
	stream.read(text.data(), count);

	return text;

}



std::string File::readWord() {

	std::string word;
	stream >> word;

	return word;

}



std::string File::readLine() {

	std::string line;
	std::getline(stream, line);

	return line;

}



std::string File::readAllText() {

	seekFromEnd(0);
	auto fileSize = stream.tellg();
	seekTo(0);

	std::string bytes(fileSize, 0);
	stream.read(bytes.data(), fileSize);

    return bytes;

}



void File::write(const std::string& text) {
	stream << text;
}



void File::writeLine(const std::string& line) {
	stream << line << '\n';
}



SizeT File::read(const std::span<u8>& data) {

	stream.read(reinterpret_cast<char*>(data.data()), data.size());
	return stream.gcount();

}


void File::write(const std::span<const u8>& data) {
	stream.write(reinterpret_cast<const char*>(data.data()), data.size());
}



std::vector<u8> File::readAll() {

	std::vector<u8> bytes(size());
	stream.read(reinterpret_cast<char*>(bytes.data()), bytes.size());

	return bytes;

}



void File::seek(i64 offset) {
	stream.seekg(offset, std::ios::cur);
}



void File::seekTo(u64 offset) {
	stream.seekg(static_cast<i64>(offset), std::ios::beg);
}



void File::seekFromEnd(u64 offset) {
	stream.seekg(static_cast<i64>(offset), std::ios::end);
}



u64 File::getPosition() const {
	return stream.tellg();
}



bool File::isOpen() const {
	return stream.is_open();
}



u32 File::getStreamFlags() const {
	return openFlags;
}



u64 File::size() const {
	return std::filesystem::file_size(path().getHandle());
}



bool File::exists() const {
	return fsEntry().exists();
}



bool File::create() {

	try {
		std::ofstream dummyStream(path().toString(), std::ios::out | std::ios::app);
	} catch (const std::exception&) {
		return false;
	}

	return exists();

}



bool File::copy(const Path& where, FSCopyExisting copyExisting) const {

	std::filesystem::copy_options options = std::filesystem::copy_options::none;

	switch (copyExisting) {

		case FSCopyExisting::Skip: options |= std::filesystem::copy_options::skip_existing; break;
		case FSCopyExisting::Update: options |= std::filesystem::copy_options::update_existing; break;
		case FSCopyExisting::Overwrite: options |= std::filesystem::copy_options::overwrite_existing; break;
		default: break;

	}

	try {
		std::filesystem::copy_file(path().getHandle(), where.getHandle(), options);
	} catch (const std::exception&) {
		return false;
	}

	return true;

}



bool File::rename(const Path& to) {
	return fsEntry().rename(to);
}



bool File::remove() {
	return fsEntry().remove();
}



Path File::path() const {
	return filePath;
}



FSEntry File::fsEntry() const {
	return FSEntry(filePath);
}