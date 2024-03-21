/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 BitStream.hpp
 */

#pragma once


#include "STDExt/BitSpan.hpp"
#include "Common/Assert.hpp"



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

	constexpr void seekTo(SizeT n) noexcept {

		arc_assert(n <= size(), "Attempted to seek out of bounds");
		cursor = n;

	}

	constexpr void setStream(const StreamType& stream) noexcept {
		this->stream = stream;
	}

	constexpr StreamType getStream() const noexcept {
		return stream;
	}

	constexpr SizeT position() const noexcept {
		return cursor;
	}

	constexpr SizeT size() const noexcept {
		return stream.size();
	}

	constexpr SizeT remainingSize() const noexcept {
		return size() - position();
	}

	constexpr ByteType* head() const noexcept {
		return stream.data() + cursor / 8;
	}

	constexpr u32 headOffset() const noexcept {
		return cursor % 8;
	}

protected:

	StreamType stream;
	SizeT cursor;

};