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
	constexpr T peek() noexcept {

		T t = Bits::assemble<T>(head());

		if (convert) {
			t = Bits::swap(t);
		}

		return t;

	}

	template<Arithmetic T>
	constexpr void peek(const std::span<T>& dest) noexcept {

		for (SizeT i = 0; i < dest.size(); i++) {
			dest[i] = peek<T>();
		}

	}

	template<Arithmetic T>
	constexpr T read() noexcept {

		T x = peek<T>();
		seek(sizeof(T));

		return x;

	}

	template<Arithmetic T>
	constexpr void read(const std::span<T>& dest) noexcept {

		for (SizeT i = 0; i < dest.size(); i++) {
			dest[i] = read<T>();
		}

	}

	constexpr BinaryReader substream(SizeT size) const noexcept {
		return BinaryReader(stream.subspan(cursor, size));
	}

private:

	bool convert;

};