#pragma once

#include <string>
#include <fstream>
#include <filesystem>

#include "types.h"


class URIPath {

public:

	typedef std::filesystem::directory_iterator DirectoryIterator;
	typedef std::filesystem::recursive_directory_iterator RecursiveDirectoryIterator;
	

	URIPath();
	URIPath(const std::string& path);

	void setPath(const std::string& path);

	bool valid() const;
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
	explicit File(const URIPath& path, File::Flags flags = File::In);

	bool open(const URIPath& path, File::Flags flags = File::In);
	void close();

	std::string read(u64 count);
	std::string readLine();
	std::string readAll();
	void write(const std::string& text);

	void read(u8* data, u64 count);
	void write(u8* data, u64 count);

	void seek(u64 pos);
	void seekRelative(i64 pos);

	bool isOpen() const;
	u64 getFileSize() const;

	URIPath getPath() const;
	Flags getStreamFlags() const;
	u64 getLastWriteTime() const;
	

private:
	std::fstream stream;
	URIPath filepath;
	Flags openFlags;

};