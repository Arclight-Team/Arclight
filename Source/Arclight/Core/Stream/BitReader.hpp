/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 BitReader.hpp
 */

#pragma once

#include "BitStream.hpp"

#include <span>



class BitReader : public BitStream<true> {

public:

	constexpr BitReader() noexcept {}
	constexpr explicit BitReader(const StreamType& stream) noexcept : BitStream(stream) {}


	template<CC::Arithmetic A>
	constexpr A read(SizeT size) noexcept {

		A a = stream.subspan(cursor, size).read<A>(size);
		seek(size);

		return a;

	}

	template<CC::Arithmetic T>
	constexpr void read(const std::span<T>& dest, SizeT size) noexcept {

		for (SizeT i = 0; i < dest.size(); i++) {
			dest[i] = read<T>(size);
		}

	}

	constexpr BitReader substream(SizeT size) const noexcept {
		return BitReader(stream.subspan(cursor, size));
	}

};