#pragma once
#include "stream.h"
#include "util/file.h"


class FileStream : public Stream
{
public:

	FileStream(File& file);

	virtual u64 read(void* dest, u64 size) override;
	virtual u64 write(const void* src, u64 size) override;

	virtual u64 seek(i64 offset, SeekMode mode = SeekMode::Begin) override;
	virtual u64 tell() const override;

	virtual u64 getSize() const override;
	virtual bool isOpen() const override;

private:

	File& file;

};