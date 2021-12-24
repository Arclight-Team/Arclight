#pragma once

#include "inputstream.hpp"
#include "filestreamimpl.hpp"


class File;

class FileInputStream : public InputStream, private FileStreamImpl
{
public:

	FileInputStream(File& file);

	virtual SizeT read(void* dest, SizeT size) override;

	virtual SizeT seek(i64 offset, SeekMode mode = SeekMode::Begin) override;
	virtual SizeT getPosition() const override;

	virtual SizeT getSize() const override;
	virtual bool isOpen() const override;
	virtual bool reachedEnd() const override;

};