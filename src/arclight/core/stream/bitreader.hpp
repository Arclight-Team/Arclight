/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitreader.hpp
 */

#pragma once

#include "math/math.hpp"
#include "stream/inputstream.hpp"
#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "util/bits.hpp"
#include "arcconfig.hpp"

#include "stdext/bitspan.hpp"

class BitReader {

public:

	BitReader(InputStream& stream) : stream(stream), strayByte(0), start(0) {}

	template<Arithmetic A, SizeT Size = Bits::bitCount<A>(), class I = TT::UnsignedFromSize<sizeof(A)>>
	requires (Size <= Bits::bitCount<A>())
	A read() {

		if constexpr (Size == 0) {
			return 0;
		}

		I i = 0;

		if (start) {

			//We have a stray byte, copy it into the LSB
			i |= strayByte;
			i >>= start;

			u8 readBits = 8 - start;

			if (Size <= readBits) {

				//Stray byte not exhausted
				i &= (1ULL << Size) - 1;

				start = (Size + start) % 8;

			} else {

				//Stray byte exhausted, read from stream
				SizeT readByteCount = Math::alignUp(Size - readBits, 8) / 8;

				I b;

				[[maybe_unused]] SizeT readBytes = stream.read(&b, readByteCount);

#ifndef ARC_STREAM_ACCELERATE
				if (readBytes != readByteCount) {
					arc_force_assert("Failed to read data from stream");
				}
#endif

				start = (Size + start) % 8;

				if (start) {
					strayByte = Bits::toByteArray(&b)[readByteCount - 1];
				}

				i |= (b & ((1ULL << (Size - readBits)) - 1)) << readBits;

			}

		} else {

			//No stray byte
			SizeT readByteCount = Math::alignUp(Size, 8) / 8;

			[[maybe_unused]] SizeT readBytes = stream.read(&i, readByteCount);

#ifndef ARC_STREAM_ACCELERATE
			if (readBytes != readByteCount) {
				arc_force_assert("Failed to read data from stream");
			}
#endif

			start = Size % 8;

			if (start) {
				strayByte = Bits::toByteArray(&i)[readByteCount - 1];
			}

			i &= (1ULL << Size) - 1;

		}

		return Bits::cast<A>(i);

	}


	void skip(u64 n) {

		if (!n) {
			return;
		}

		u64 newPos = start + n;
		bool fetchStray = true;

		i64 seekCount = static_cast<i64>(newPos) / 8;

		if (seekCount) {

			stream.seek(seekCount);

		} else if (start != 0) {

			fetchStray = false;

		}

		start = newPos % 8;

		if (fetchStray) {

			[[maybe_unused]] SizeT readBytes = stream.read(&strayByte, 1);

#ifndef ARC_STREAM_ACCELERATE
			if (readBytes != 1) {
				arc_force_assert("Failed to read data from stream");
			}
#endif

		}

	}


	u64 getBitPosition() const {
		return stream.getPosition() * 8 + start;
	}

	u64 getPosition() const {
		return stream.getPosition();
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