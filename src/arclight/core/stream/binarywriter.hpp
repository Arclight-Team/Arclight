/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 binarywriter.hpp
 */

#pragma once

#include "binarystream.hpp"
#include "util/bits.hpp"

#include <span>



class BinaryWriter : public BinaryStream<false> {

public:

	constexpr BinaryWriter() noexcept : convert(false) {}
	constexpr explicit BinaryWriter(const std::span<ByteType>& stream, ByteOrder order = ByteOrder::Little) noexcept : BinaryStream(stream), convert(Bits::requiresEndianConversion(order)) {}

	template<CC::Arithmetic T>
	constexpr void write(TT::RemoveConst<T> t) noexcept {

		if (convert) {
			t = Bits::swap(t);
		}

		ByteType* ptr = head();
		Bits::disassemble(t, ptr);

		seek(sizeof(T));

	}

	template<CC::Arithmetic T>
	constexpr void write(const std::span<T>& src) noexcept {

		for (SizeT i = 0; i < src.size(); i++) {
			write<T>(src[i]);
		}

	}

	constexpr BinaryWriter substream(SizeT size) const noexcept {
		return BinaryWriter(stream.subspan(cursor, size));
	}

private:

	bool convert;

};