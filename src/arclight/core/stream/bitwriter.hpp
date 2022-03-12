/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bitwriter.hpp
 */

#pragma once

#include "bitstream.hpp"

#include <span>



class BitWriter : public BitStream<false> {

public:

	constexpr BitWriter() noexcept {}
	constexpr explicit BitWriter(const StreamType& stream) noexcept : BitStream(stream) {}


	template<Arithmetic A>
	constexpr void write(SizeT size, A a) noexcept {

		stream.subspan(cursor, size).write<A>(size, a);
		seek(size);

	}

	template<Arithmetic T>
	constexpr void write(SizeT size, const std::span<T>& dest) noexcept {

		for (SizeT i = 0; i < dest.size(); i++) {
			write<T>(size, dest[i]);
		}

	}

};