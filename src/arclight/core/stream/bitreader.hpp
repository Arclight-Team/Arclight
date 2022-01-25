/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitreader.hpp
 */

#pragma once

#include "math/math.hpp"
#include "stdext/bitspan.hpp"
#include "stream/inputstream.hpp"
#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "util/bits.hpp"
#include "arcconfig.hpp"



/*
 *  BitReader allows reading bits from a stream
 *  Will not take multiple readers/writers into account.
 */
class BitReader {

public:

	BitReader(InputStream& stream) : stream(stream), strayByte(0), start(0) {}



	template<Arithmetic A, SizeT Size = Bits::bitCount<A>()> requires (Size <= Bits::bitCount<A>())
	A read() {

		using I = TT::UnsignedFromSize<sizeof(A)>;

		if constexpr (!Size) {
			return A(I(0));
		}

		alignas(I) u8 buffer[sizeof(I) * 2];
		u32 sidx = sizeof(I);
		u32 bidx = sizeof(I);

		if (start) {
			buffer[--sidx] = strayByte;
		}

		u32 startBits = (8 - start) % 8;
		SizeT alignedReadBytes = Math::alignUp(Size - startBits, 8) / 8;

		if (alignedReadBytes) {

			[[maybe_unused]] SizeT readBytes = stream.read(&buffer[bidx], alignedReadBytes);

#ifndef ARC_STREAM_ACCELERATE
			if (readBytes != alignedReadBytes) {
				arc_force_assert("Failed to read data from stream");
			}
#endif

		}

		SizeT endBits = (start + Size) % 8;

		if (endBits) {
			strayByte = buffer[bidx + alignedReadBytes - 1];
		}

		BitSpan<Size, false> span {buffer + sidx, start, Size};

		start = endBits;

		return span.read<A, Size>();

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