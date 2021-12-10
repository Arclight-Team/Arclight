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

	virtual SizeT seek(i64 offset, SeekMode mode = SeekMode::Begin) override;
	virtual SizeT getPosition() const override;

	virtual SizeT getSize() const override;
	virtual bool isOpen() const override;
    virtual bool reachedEnd() const override;

    auto substream(SizeT offset, SizeT size = SizeT(-1)) {

        if (size == SizeT(-1)) {
            SizeT remaining = getData().size() - offset;
            return ByteInputStream(getData().subspan(offset, remaining));
        }
        else {
            arc_assert(size < getData().size(), "Substream size is greater than the stream' size");
            return ByteInputStream(getData().subspan(offset, size));
        }

    }


};