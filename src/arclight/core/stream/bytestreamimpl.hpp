/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bytestreamimpl.hpp
 */

#pragma once

#include "streambase.hpp"
#include "util/assert.hpp"
#include "util/bits.hpp"
#include "util/typetraits.hpp"
#include "math/math.hpp"
#include "types.hpp"

#include <span>
#include <vector>
#include <memory>


template<bool Const, bool Dynamic = false>
class ByteStreamImpl
{
public:

    static_assert(!(Const && Dynamic));

    using u8Type = TT::ConditionalConst<Const, u8>;
    using StorageType = TT::Conditional<Dynamic, std::vector<u8Type>, std::span<u8Type>>;

	ByteStreamImpl() requires(Dynamic) = default;

	template<class T>
	ByteStreamImpl(const std::span<T>& data) : position(0)  {

        if constexpr (!Dynamic) {

			//convert T span to byte span
            if constexpr (Const) {
                this->data = StorageType{ Bits::toByteArray(data.data()), data.size_bytes() };
			}
			else {
                this->data = StorageType{ Bits::toByteArray(data.data()), data.size_bytes() };
			}

        }
        else {

            this->data.resize(data.size() * sizeof(T));
            std::memcpy(this->data.data(), data.data(), this->data.size());

        }


	}

	SizeT read(void* dest, SizeT size) {

        arc_assert(dest != nullptr, "Destination is null");
        arc_assert(position + size <= getSize(), "Cannot read past the end of the stream");

        size = Math::min(size, getSize() - position);
        std::copy_n(data.data() + position, size, static_cast<u8*>(dest));
        
        position += size;

        return size;

    }

	SizeT write(const void* src, SizeT size) requires (!Const && !Dynamic) {

        arc_assert(src != nullptr, "Source is null");
        arc_assert(position + size <= getSize(), "Cannot write past the end of the stream");

        size = Math::min(size, getSize() - position);
        std::copy_n(static_cast<const u8*>(src), size, data.data() + position);

        position += size;

        return size;

    }

    SizeT write(const void* src, SizeT size) requires (!Const && Dynamic) {

        arc_assert(src != nullptr, "Source is null");

        // Resize if needed
        if (position + size > getSize()) {
            data.resize(position + size);
        }

        size = Math::min(size, getSize() - position);
        std::memcpy(data.data() + position, src, size);

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

    constexpr auto getData() {
        return data;
    }

    constexpr auto getData() const {
        return data;
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

    StorageType data;
    SizeT position;

};

template<bool Dynamic>
using ByteStreamImplRW  = ByteStreamImpl<false, Dynamic>;
using ByteStreamImplR   = ByteStreamImpl<true>;