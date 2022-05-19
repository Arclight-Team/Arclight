/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 syncbuffer.hpp
 */

#pragma once

#include "memory/alignedallocator.hpp"
#include "math/vector.hpp"
#include "math/matrix.hpp"
#include "types.hpp"

#include <span>
#include <vector>



class SyncBuffer {

	using Buffer = std::vector<u8, AlignedAllocator<u8, 32>>;

public:

	SyncBuffer();

	bool hasChanged() const noexcept;
	SizeT size() const noexcept;

	void resize(SizeT newSize);
	void clear();

	const u8* data() const noexcept;
	const u8& operator[](SizeT offset) const;

	const u8* updateStartData() const;

	SizeT getUpdateStart() const noexcept;
	SizeT getUpdateEnd() const noexcept;
	SizeT getUpdateSize() const noexcept;

	template<CC::Arithmetic T>
	void write(SizeT offset, T t) {

		write(offset, {Bits::toByteArray(&t), sizeof(T)});
		updateBounds(offset, sizeof(T));

	}

	template<CC::Vector V>
	void write(SizeT offset, const V& v) {

		constexpr u32 vecSize = V::Size;
		using T = typename V::Type;

		for (u32 i = 0; i < vecSize; i++) {
			write(offset + i * sizeof(T), {Bits::toByteArray(&v[i]), sizeof(T)});
		}

		updateBounds(offset, vecSize * sizeof(T));

	}

	template<CC::Matrix M>
	void write(SizeT offset, const M& m) {

		constexpr u32 matSize = M::Size;
		using T = typename M::Type;

		for (u32 i = 0; i < matSize; i++) {

			for (u32 j = 0; j < matSize; j++) {
				write(offset + (i * matSize + j) * sizeof(T), {Bits::toByteArray(&m[i][j]), sizeof(T)});
			}

		}

		updateBounds(offset, matSize * matSize * sizeof(T));

	}

	void write(SizeT offset, const std::span<const u8>& source);

	void updateBounds(SizeT offset, SizeT size) noexcept;
	void finishUpdate() noexcept;

private:

	SizeT updateStart, updateEnd;
	Buffer buffer;

};