#pragma once

#include "streambase.hpp"


class File;

class FileStreamImpl {
    
public:

    FileStreamImpl(File& file);

	SizeT read(void* dest, SizeT size);
	SizeT write(const void* src, SizeT size);

    SizeT seek(i64 offset, StreamBase::SeekMode mode);
	SizeT getPosition() const;
    SizeT getSize() const;

	bool isOpen() const;
	bool reachedEnd() const;

private:

	File& file;

};