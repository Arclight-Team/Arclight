#pragma once

#include "bytestreamimpl.h"
#include "stream.h"
#include <span>


class ByteStream : public Stream, private ByteStreamImplRW
{
public:

    using StreamBase::SeekMode;

    template<class T>
    ByteStream(std::span<T>& data) : ByteStreamImplRW(data) {}

	virtual u64 read(void* dest, u64 size) override;
	virtual u64 write(const void* src, u64 size) override;

	virtual u64 seek(i64 offset, SeekMode mode) override;
	virtual u64 getPosition() const override;

	virtual u64 getSize() const override;
	virtual bool isOpen() const override;
    
};