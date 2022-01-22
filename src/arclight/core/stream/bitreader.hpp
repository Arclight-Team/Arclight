/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitreader.hpp
 */

#pragma once

#include "stdext/bitspan.hpp"
#include "stream/inputstream.hpp"
#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "arcconfig.hpp"



class BitReader {

public:

	BitReader(InputStream& stream) : stream(stream), strayByte(0), start(0) {}

	template<Integral I, SizeT Size = Bits::bitCount<I>()> requires (Size <= Bits::bitCount<I>())
	I read() {

		if constexpr (Size == 0) {
			return 0;
		}

		I i = 0;

		if (start) {

			//We have i stray byte, copy it into the LSB
			i |= strayByte;
			i >>= start;

			u8 readBits = 8 - start;

			if (Size <= readBits) {

				//Stray byte not exhausted
				i &= (1ULL << Size) - 1;

				start = (Size + start) % 8;

			} else {

				//Stray byte exhausted, read from stream
				SizeT ReadByteCount = Math::alignUp(Size - readBits, 8) / 8;

				I b;

				[[maybe_unused]] SizeT readBytes = stream.read(&b, ReadByteCount);

#ifndef ARC_STREAM_ACCELERATE
	            if (readBytes != ReadByteCount) {
	                arc_force_assert("Failed to read data from stream");
	            }
#endif

				start = (Size + start) % 8;

				if (start) {
					strayByte = Bits::toByteArray(&b)[ReadByteCount - 1];
				}

				i |= (b & ((1ULL << (Size - readBits)) - 1)) << readBits;

			}

		} else {

			SizeT ReadByteCount = Math::alignUp(Size, 8) / 8;

			[[maybe_unused]] SizeT readBytes = stream.read(&i, ReadByteCount);

#ifndef ARC_STREAM_ACCELERATE
            if (readBytes != ReadByteCount) {
                arc_force_assert("Failed to read data from stream");
            }
#endif

			start = Size % 8;

			if (start) {
				strayByte = Bits::toByteArray(&i)[ReadByteCount - 1];
			}

			i &= (1ULL << Size) - 1;

		}

		return i;

	}


	void skip(SizeT n) {

		SizeT seekCount = (start + n) / 8;
		start = (start + n) % 8;
		seekCount += !!start;

		if (seekCount) {
			stream.seek(seekCount, StreamBase::SeekMode::Current);
		}

	}


    InputStream& getStream() {
        return stream;
    }

    const InputStream& getStream() const {
        return stream;
    }

private:

	InputStream& stream;
	u8 strayByte;
	u8 start;

};