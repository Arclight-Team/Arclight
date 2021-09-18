#pragma once

#include "bytestreamimpl.h"
#include "stream.h"
#include <span>


class ByteStream : public Stream, private ByteStreamImplRW
{
public:

    using StreamBase::SeekMode;

    template<class T> requires Equal<T, std::remove_cv_t<T>>
    ByteStream(const std::span<T>& data) : ByteStreamImplRW(data) {}

	virtual SizeT read(void* dest, SizeT size) override;
	virtual SizeT write(const void* src, SizeT size) override;

	virtual SizeT seek(i64 offset, SeekMode mode) override;
	virtual SizeT getPosition() const override;

	virtual SizeT getSize() const override;
	virtual bool isOpen() const override;
    virtual bool reachedEnd() const override;

};