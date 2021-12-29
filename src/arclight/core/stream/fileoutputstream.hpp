/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 fileoutputstream.hpp
 */

#pragma once

#include "outputstream.hpp"
#include "filestreamimpl.hpp"


class File;

class FileOutputStream : public OutputStream, private FileStreamImpl
{
public:

	FileOutputStream(File& file);

	virtual SizeT write(const void* src, SizeT size) override;

	virtual SizeT seek(i64 offset, SeekMode mode = SeekMode::Begin) override;
	virtual SizeT getPosition() const override;

	virtual SizeT getSize() const override;
	virtual bool isOpen() const override;
	virtual bool reachedEnd() const override;

};