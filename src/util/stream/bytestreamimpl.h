#pragma once

#include "streambase.h"
#include "util/assert.h"
#include "util/math.h"
#include "types.h"
#include <span>
#include <memory>


template<bool Const>
class ByteStreamImpl
{    
public:

    using ByteType = std::conditional_t<Const, const std::byte, std::byte>;

	template<class T>
	ByteStreamImpl(const std::span<T>& data) : position(0) {

		//convert T span to byte span
        if constexpr (Const) {
            this->data = data.as_bytes();
        } else {
            this->data = data.as_writeable_bytes();
        }

	}

	u64 read(void* dest, u64 size) {

        arc_assert(dest != nullptr, "Destination is null");
        arc_assert(position + size <= getSize(), "Cannot read past the end of the stream");

        size = Math::min(size, getSize() - position);
        std::memcpy(dest, data.data() + position, size);
        
        position += size;

        return size;

    }

	u64 write(const void* src, u64 size) requires !Const {

        arc_assert(src != nullptr, "Source is null");
        arc_assert(position + size <= getSize(), "Cannot write past the end of the stream");

        size = Math::min(size, getSize() - position);
        std::memcpy(data.data() + position, src, size);

        position += size;

        return size;

    }

    u64 seek(i64 offset, StreamBase::SeekMode mode) {

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

	u64 getPosition() const {
        return position;
    }

    u64 getSize() const {
        return data.size();
    }

	bool isOpen() const {
        return data.size();
    }

private:

    std::span<ByteType> data;
    u64 position;

};

using ByteStreamImplRW = ByteStreamImpl<false>;
using ByteStreamImplR = ByteStreamImpl<true>;