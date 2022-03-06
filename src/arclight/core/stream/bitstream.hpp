/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitstream.hpp
 */

#pragma once


#include "stdext/bitspan.hpp"
#include "util/assert.hpp"



template<bool Const>
class BitStream {

public:

	using ByteType = TT::ConditionalConst<Const, u8>;
	using StreamType = BitSpan<std::dynamic_extent, Const>;


	constexpr BitStream() noexcept : cursor(0) {}
	constexpr explicit BitStream(const StreamType& stream) noexcept : stream(stream), cursor(0) {}


	constexpr void seek(i64 n) noexcept {

		arc_assert(i64(cursor) + n >= 0 && cursor + n <= size(), "Attempted to seek out of bounds");
		cursor += n;

	}

	constexpr void seekTo(u64 n) noexcept {

		arc_assert(n <= size(), "Attempted to seek out of bounds");
		cursor = n;

	}

	constexpr void setStream(const StreamType& stream) noexcept {
		this->stream = stream;
	}

	constexpr StreamType getStream() const noexcept {
		return stream;
	}

	constexpr u64 position() const noexcept {
		return cursor;
	}

	constexpr u64 size() const noexcept {
		return stream.size();
	}

	constexpr u64 remainingSize() const noexcept {
		return size() - position();
	}

	constexpr ByteType* head() const noexcept {
		return stream.data() + cursor / 8;
	}

	constexpr u32 headOffset() const noexcept {
		return cursor % 8;
	}

	constexpr BitStream<Const> substream(u64 size) const noexcept {
		return BitStream<Const>(stream.subspan(cursor, size));
	}

protected:

	StreamType stream;
	u64 cursor;

};