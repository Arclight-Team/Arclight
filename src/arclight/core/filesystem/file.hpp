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
#include <vector>



class File {

public:

	enum Flags : u32 {
		In = 0x1,
		Out = 0x2,
		Text = 0x4,
		AtEnd = 0x8,
		Append = 0x10,
		Trunc = 0x20
	};

	File();
	File(const Path& path, u32 flags = File::In);

	bool open();
	bool open(const Path& path, u32 flags = File::In);
	void close();

	std::string readChars(u64 count);
	std::string readWord();
	std::string readLine();
	std::string readAllText();

	void write(const std::string& text);
	void writeLine(const std::string& line);

	SizeT read(const std::span<u8>& data);
	void write(const std::span<const u8>& data);
	std::vector<u8> readAll();

	void seek(i64 offset);
	void seekTo(u64 offset);
	void seekFromEnd(u64 offset);

	u64 getPosition() const;

	bool isOpen() const;
	u64 size() const;

	u32 getStreamFlags() const;


	bool exists() const;

	bool create();
	bool copy(const Path& where, FSCopyExisting copyExisting = FSCopyExisting::Skip) const;
	bool rename(const Path& to);
	bool remove();

	Path path() const;
	FSEntry fsEntry() const;

private:

	mutable std::fstream stream;
	Path filePath;
	u32 openFlags;

};