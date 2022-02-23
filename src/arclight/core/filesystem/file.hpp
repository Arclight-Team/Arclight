/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 file.hpp
 */

#pragma once

#include "path.hpp"
#include "types.hpp"
#include "fsentry.hpp"

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
	File(FSEntry entry, u32 flags = File::In);
	File(const Path& path, u32 flags = File::In);

	bool open();
	bool open(const Path& path, u32 flags = File::In);
	void close();

	std::string read(u64 count);
	std::string readWord();
	std::string readLine();
	std::string readAll();
	void write(const std::string& text);
	void writeLine(const std::string& line);

	SizeT read(const std::span<u8>& data);
	void write(const std::span<const u8>& data);

	void seek(i64 offset);
	void seekTo(u64 offset);

	u64 getPosition() const;

	bool isOpen() const;
	u64 getFileSize() const;

	u32 getStreamFlags() const;


	bool exists();

	bool create();
	bool copy(const Path& where, FSCopyExisting copyExisting = FSCopyExisting::Skip);
	bool rename(const Path& to);
	bool remove();

	Path getPath() const;
	FSEntry getFSEntry() const;

private:

	mutable std::fstream stream;
	FSEntry entry;
	u32 openFlags;

};