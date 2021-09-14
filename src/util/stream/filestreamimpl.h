#pragma once

#include "streambase.h"


class File;

class FileStreamImpl {
    
public:

    FileStreamImpl(File& file);

	u64 read(void* dest, u64 size);
	u64 write(const void* src, u64 size);

    u64 seek(i64 offset, StreamBase::SeekMode mode);
	u64 getPosition() const;
    u64 getSize() const;

	bool isOpen() const;

private:

	File& file;

};