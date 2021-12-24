#pragma once

// TRANSITION
#include "util/uri.hpp"
#include "types.hpp"

#include <string>
#include <fstream>
#include <span>



class File {

public:

	enum Flags : u32 {
		In = 0x1,
		Out = 0x2,
		Binary = 0x4,
		AtEnd = 0x8,
		Append = 0x10,
		Trunc = 0x20
	};

	File();
	explicit File(const Uri& path, u32 flags = File::In);

	bool open();
	bool open(const Uri& path, u32 flags = File::In);
	void close();

	std::string read(u64 count);
	std::string readWord();
	std::string readLine();
	std::string readAll();
	void write(const std::string& text);
	void writeLine(const std::string& line);

	SizeT read(const std::span<u8>& data);
	void write(const std::span<const u8>& data);

	u64 tell() const;
	void seek(u64 pos);
	void seekRelative(i64 pos);


	bool isOpen() const;
	u64 getFileSize() const;

	Uri getUri() const;
	u32 getStreamFlags() const;
	u64 getLastWriteTime() const;
	
private:

	mutable std::fstream stream;
	Uri filepath;
	u32 openFlags;

};