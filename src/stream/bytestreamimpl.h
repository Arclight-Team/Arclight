#pragma once

#include "streambase.h"
#include "util/assert.h"
#include "math/math.h"
#include "types.h"

#include <span>
#include <memory>
#include <algorithm>


template<bool Const>
class ByteStreamImpl
{    
public:

    using ByteType = std::conditional_t<Const, const Byte, Byte>;

	template<class T>
	ByteStreamImpl(const std::span<T>& data) : position(0) {

		//convert T span to byte span
        if constexpr (Const) {
            this->data = std::as_bytes(data);
        } else {
            this->data = std::as_writable_bytes(data);
        }

	}

	SizeT read(void* dest, SizeT size) {

        arc_assert(dest != nullptr, "Destination is null");
        arc_assert(position + size <= getSize(), "Cannot read past the end of the stream");

        size = Math::min(size, getSize() - position);
        std::copy_n(data.data() + position, size, static_cast<Byte*>(dest));
        
        position += size;

        return size;

    }

	SizeT write(const void* src, SizeT size) requires (!Const) {

        arc_assert(src != nullptr, "Source is null");
        arc_assert(position + size <= getSize(), "Cannot write past the end of the stream");

        size = Math::min(size, getSize() - position);
        std::copy_n(static_cast<const Byte*>(src), size, data.data() + position);

        position += size;

        return size;

    }

    SizeT seek(i64 offset, StreamBase::SeekMode mode) {

        switch (mode) {

            case StreamBase::SeekMode::Begin:
                arc_assert(offset >= 0, "Cannot seek before the start of the stream");
                position = offset;
                break;

            case StreamBase::SeekMode::Current:
                position = getPosition() + offset;
                break;

            case StreamBase::SeekMode::End:
                arc_assert(offset >= 0, "Cannot seek after the end of the stream");
                position = getSize() - offset;
                break;

            }

        return position;

    }

	SizeT getPosition() const {
        return position;
    }

    SizeT getSize() const {
        return data.size();
    }

	bool isOpen() const {
        return data.size();
    }

    bool reachedEnd() const {
        return position == data.size();
    }

private:

    std::span<ByteType> data;
    SizeT position;

};

using ByteStreamImplRW = ByteStreamImpl<false>;
using ByteStreamImplR = ByteStreamImpl<true>;