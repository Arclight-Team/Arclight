/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 filestreamimpl.hpp
 */

#pragma once

#include "streambase.hpp"


class File;

class FileStreamImpl {
	
public:

	FileStreamImpl(File& file);

	SizeT read(void* dest, SizeT size);
	SizeT write(const void* src, SizeT size);

	void seek(i64 offset);
	void seekTo(u64 offset);
	SizeT getPosition() const;
	SizeT getSize() const;

	bool isOpen() const;
	bool reachedEnd() const;

private:

	File& file;

};