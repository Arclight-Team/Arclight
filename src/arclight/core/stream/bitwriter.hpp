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



/*
 *  BitWriter allows writing bits to a stream
 *  Will not take previous memory contents and multiple readers/writers into account.
 */
class BitWriter {

public:

	BitWriter(OutputStream& stream) : stream(stream), strayByte(0), start(0) {}

	template<Arithmetic A, SizeT Size = Bits::bitCount<A>()> requires (Size <= Bits::bitCount<A>())
	void write(A a) {

		using I = TT::UnsignedFromSize<sizeof(A)>;

		if constexpr (!Size) {
			return;
		}

		I i = Bits::cast<I>(a);

		alignas(I) u8 buffer[sizeof(I) * 2];
		u32 sidx = sizeof(I);
		u32 bidx = sizeof(I);
		SizeT endBits = (start + Size) % 8;

		if (start) {
			buffer[--sidx] = strayByte;
		}

		u32 startBits = (8 - start) % 8;
		SizeT alignedWriteBytes = Math::alignUp(Size - startBits, 8) / 8;
		SizeT writeBytes = alignedWriteBytes + !!start;

		bool hasEndByte = endBits && alignedWriteBytes;
		u32 endBytePos = bidx + alignedWriteBytes - 1;

		if (hasEndByte) {
			buffer[endBytePos] = 0;
		}

		BitSpan<Size, false> span {buffer + sidx, start, Size};

		start = endBits;

		span.write<Size>(i);

		if (writeBytes) {

			[[maybe_unused]] SizeT writtenBytes = stream.write(&buffer[sidx], writeBytes);

#ifndef ARC_STREAM_ACCELERATE
			if (writtenBytes != writeBytes) {
				arc_force_assert("Failed to write data to stream");
			}
#endif

		}

		if (hasEndByte) {

			strayByte = buffer[endBytePos];
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