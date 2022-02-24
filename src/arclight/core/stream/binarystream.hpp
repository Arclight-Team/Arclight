/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 binarystream.hpp
 */


#pragma once

#include "util/assert.hpp"
#include "util/typetraits.hpp"

#include <span>



template<bool Const>
class BinaryStream {

public:

	using ByteType = TT::ConditionalConst<Const, u8>;

	constexpr BinaryStream() noexcept : cursor(0) {}
	constexpr explicit BinaryStream(const std::span<ByteType>& stream) noexcept : stream(stream), cursor(0) {}


	constexpr void seek(i64 n) noexcept {

		arc_assert(i64(cursor) + n >= 0 && cursor + n <= getSize(), "Attempted to seek out of bounds");
		cursor += n;

	}

	constexpr void seekTo(u64 n) noexcept {

		arc_assert(n <= getSize(), "Attempted to seek out of bounds");
		cursor = n;

	}

	constexpr void setStream(const std::span<const u8>& stream) noexcept {
		this->stream = stream;
	}

	constexpr std::span<const u8> getStream() const noexcept {
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

protected:

	constexpr ByteType* head() const noexcept {
		return stream.data() + cursor;
	}

	std::span<ByteType> stream;
	u64 cursor;

};