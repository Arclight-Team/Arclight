#pragma once

#include "bytestreamimpl.h"
#include "inputstream.h"
#include <span>


class ByteInputStream : public InputStream, private ByteStreamImplR
{
public:

    using StreamBase::SeekMode;

    template<class T>
    ByteInputStream(const std::span<T>& data) : ByteStreamImplR(data) {}

	virtual SizeT read(void* dest, SizeT size) override;

	virtual SizeT seek(i64 offset, SeekMode mode) override;
	virtual SizeT getPosition() const override;

	virtual SizeT getSize() const override;
	virtual bool isOpen() const override;
    virtual bool reachedEnd() const override;

};