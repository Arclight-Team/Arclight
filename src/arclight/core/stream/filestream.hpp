/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 filestream.hpp
 */

#pragma once

#include "stream.hpp"
#include "filestreamimpl.hpp"


class File;

class FileStream : public Stream, private FileStreamImpl
{
public:

	FileStream(File& file);

	virtual SizeT read(void* dest, SizeT size) override;
	virtual SizeT write(const void* src, SizeT size) override;

	virtual void seek(i64 offset) override;
	virtual void seekTo(u64 offset) override;
	virtual SizeT getPosition() const override;

	virtual SizeT getSize() const override;
	virtual bool isOpen() const override;
	virtual bool reachedEnd() const override;

};