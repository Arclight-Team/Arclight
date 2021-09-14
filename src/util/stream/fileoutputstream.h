#pragma once

#include "outputstream.h"
#include "filestreamimpl.h"


class File;

class FileOutputStream : public OutputStream, private FileStreamImpl
{
public:

	FileOutputStream(File& file);

	virtual u64 write(const void* src, u64 size) override;

	virtual u64 seek(i64 offset, SeekMode mode = SeekMode::Begin) override;
	virtual u64 getPosition() const override;

	virtual u64 getSize() const override;
	virtual bool isOpen() const override;

};