#pragma once

#include "outputstream.h"
#include "bytestreamimpl.h"
#include <span>


class ByteOutputStream : public OutputStream, private ByteStreamImplRW
{
public:

    using StreamBase::SeekMode;

    template<class T>
    ByteOutputStream(std::span<T>& data) : ByteStreamImplRW(data) {}

	virtual u64 write(const void* src, u64 size) override;

	virtual u64 seek(i64 offset, SeekMode mode) override;
	virtual u64 getPosition() const override;

	virtual u64 getSize() const override;
	virtual bool isOpen() const override;
    
};