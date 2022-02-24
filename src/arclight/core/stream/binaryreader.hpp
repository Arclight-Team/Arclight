/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 binaryreader.hpp
 */

#pragma once

#include "binarystream.hpp"
#include "util/bits.hpp"

#include <span>



class BinaryReader : public BinaryStream<true> {

public:

	constexpr BinaryReader() noexcept : convert(false) {}
	constexpr explicit BinaryReader(const std::span<ByteType>& stream, ByteOrder order = ByteOrder::Little) noexcept : BinaryStream(stream), convert(Bits::requiresEndianConversion(order)) {}


	template<Arithmetic T>
	constexpr T read() noexcept {

		T t = Bits::assemble<T>(head());
		seek(sizeof(T));

		if (convert) {
			t = Bits::swap(t);
		}

		return t;

	}

	template<Arithmetic T>
	constexpr void read(const std::span<T>& dest) noexcept {

		for (SizeT i = 0; i < dest.size(); i++) {
			dest[i] = read<T>();
		}

	}

private:

	bool convert;

};