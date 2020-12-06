#pragma once

#include <string>
#include <fstream>
#include <filesystem>

#include "types.h"


class URI {

public:

	typedef std::filesystem::directory_iterator DirectoryIterator;
	typedef std::filesystem::recursive_directory_iterator RecursiveDirectoryIterator;
	

	URI();
	URI(const char* path);
	URI(const std::string& path);

	void setPath(const std::string& path);
	bool createDirectory();
	void move(const std::string& path);

	bool validFile() const;
	bool validDirectory() const;
	std::string getPath() const;


private:
	std::filesystem::path path;

};




class File {

public:

	typedef u8 Flags;

	static constexpr u8 In		= std::ios::in;
	static constexpr u8 Out		= std::ios::out;
	static constexpr u8 Binary	= std::ios::binary;
	static constexpr u8 AtEnd	= std::ios::ate;
	static constexpr u8 Append	= std::ios::app;
	static constexpr u8 Trunc	= std::ios::trunc;

	File();
	explicit File(const URI& path, File::Flags flags = File::In);

	bool open(const URI& path, File::Flags flags = File::In);
	void close();

	std::string read(u64 count);
	std::string readWord();
	std::string readLine();
	std::string readAll();
	void write(const std::string& text);
	void writeLine(const std::string& line);

	void read(u8* data, u64 count);
	void write(u8* data, u64 count);

	void seek(u64 pos);
	void seekRelative(i64 pos);

	bool isOpen() const;
	u64 getFileSize() const;

	URI getURI() const;
	Flags getStreamFlags() const;
	u64 getLastWriteTime() const;
	

private:
	std::fstream stream;
	URI filepath;
	Flags openFlags;

};