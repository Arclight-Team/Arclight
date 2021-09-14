#pragma once

#include "bytestreamimpl.h"
#include "inputstream.h"
#include <span>


class ByteInputStream : public InputStream, private ByteStreamImplR
{
public:

    using StreamBase::SeekMode;

    template<class T>
    ByteInputStream(std::span<const T>& data) : ByteStreamImplR(data) {}

	virtual u64 read(void* dest, u64 size) override;

	virtual u64 seek(i64 offset, SeekMode mode) override;
	virtual u64 getPosition() const override;

	virtual u64 getSize() const override;
	virtual bool isOpen() const override;
    
};