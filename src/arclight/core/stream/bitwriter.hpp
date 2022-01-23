/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitwriter.hpp
 */

#pragma once

#include "math/math.hpp"
#include "stream/outputstream.hpp"
#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "util/bits.hpp"
#include "arcconfig.hpp"



class BitWriter {

public:

	BitWriter(OutputStream& stream) : stream(stream), strayByte(0), start(0) {}

	template<Arithmetic A, SizeT Size = Bits::bitCount<A>()> requires (Size <= Bits::bitCount<A>())
	void write(A a) {

		if constexpr (Size == 0) {
			return;
		}

		using I = TT::UnsignedFromSize<sizeof(A)>;
		I i = Bits::cast<I>(a);

		u8 newStart = (start + Size) % 8;

		i &= (1ULL << Size) - 1;

		if (start) {

			u8 writeBits = 8 - start;

			if (Size <= writeBits) {

				//Stray byte not exhausted
				u8 byte = strayByte | (i << start);

				[[maybe_unused]] SizeT writtenBytes = stream.write(&byte, 1);

#ifndef ARC_STREAM_ACCELERATE
				if (writtenBytes != 1) {
					arc_force_assert("Failed to write data to stream");
				}
#endif

				if (writeBits != Size) {
					strayByte = byte;
				} else {
					strayByte = 0;
				}

			} else {

				//Stray byte exhausted, write multiple bytes
				//Double buffer to force misalignment
				alignas(I) u8 buffer[alignof(I) * 2];

				buffer[alignof(I) - 1] = strayByte | ((i << start) & 0xFF);
				i >>= writeBits;

				SizeT writeByteCount = Math::alignUp(Size - writeBits, 8) / 8 + 1;
				std::copy_n(Bits::toByteArray(&i), sizeof(I), &buffer[alignof(I)]);

				[[maybe_unused]] SizeT writtenBytes = stream.write(buffer + alignof(I) - 1, writeByteCount);

#ifndef ARC_STREAM_ACCELERATE
				if (writtenBytes != writeByteCount) {
					arc_force_assert("Failed to write data to stream");
				}
#endif

				if (newStart) {
					strayByte = buffer[alignof(I) + writeByteCount - 2];
				}

			}

		} else {

			//No stray byte
			SizeT writeByteCount = Math::alignUp(Size, 8) / 8;

			[[maybe_unused]] SizeT writtenBytes = stream.write(&i, writeByteCount);

#ifndef ARC_STREAM_ACCELERATE
			if (writtenBytes != writeByteCount) {
				arc_force_assert("Failed to write data to stream");
			}
#endif

			if (newStart) {
				strayByte = Bits::toByteArray(&i)[writeByteCount - 1];
			}

		}

		start = newStart;

		if (start != 0) {
			stream.seek(-1);
		}

	}


	void skip(u64 n) {

		u64 newPos = start + n;
		start = newPos % 8;

		if (newPos < 8) {
			return;
		}

		i64 seekCount = static_cast<i64>(newPos) / 8;

		if (seekCount) {

			stream.seek(seekCount);

			if (start != 0) {
				strayByte = 0;
			}

		}

	}


	u64 getBitPosition() const {
		return getPosition() * 8 + start;
	}

	u64 getPosition() const {
		return stream.getPosition();
	}

	OutputStream& getStream() {
		return stream;
	}

	const OutputStream& getStream() const {
		return stream;
	}

private:

	OutputStream& stream;
	u8 strayByte;
	u8 start;

};