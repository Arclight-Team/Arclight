/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 BinaryStream.hpp
 */


#pragma once

#include "Common/Assert.hpp"
#include "Meta/TypeTraits.hpp"

#include <span>



template<bool Const>
class BinaryStream {

public:

	using ByteType = TT::ConditionalConst<Const, u8>;

	constexpr BinaryStream() noexcept : cursor(0) {}
	constexpr explicit BinaryStream(const std::span<ByteType>& stream) noexcept : stream(stream), cursor(0) {}


	constexpr void seek(i64 n) noexcept {

		arc_assert(i64(cursor) + n >= 0 && cursor + n <= size(), "Attempted to seek out of bounds");
		cursor += n;

	}

	constexpr void seekTo(SizeT n) noexcept {

		arc_assert(n <= size(), "Attempted to seek out of bounds");
		cursor = n;

	}

	constexpr void setStream(const std::span<ByteType>& stream) noexcept {
		this->stream = stream;
	}

	constexpr std::span<ByteType> getStream() const noexcept {
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
		return stream.data() + cursor;
	}

protected:

	std::span<ByteType> stream;
	SizeT cursor;

};