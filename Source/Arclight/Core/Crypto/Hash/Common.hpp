/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Common.hpp
 */

#pragma once

#include "Util/Bits.hpp"
#include "Common/Types.hpp"

#include <span>
#include <algorithm>



namespace Crypto {

	template<SizeT Size> requires (Size == 64 || Size == 128)
	struct MDConstruction {

		u8 prevLastBlock[Size];
		u8 lastBlock[Size];
		SizeT blocks;
		bool prevBlockUsed;

	};

	template<SizeT Size>
	constexpr void mdConstruct(MDConstruction<Size>& construct, const std::span<const u8>& data, ByteOrder order) {

		constexpr bool bits64 = Size == 128;
		SizeT blockSize = (bits64 ? 1024 : 512) / 8;
		SizeT padOffset = blockSize - (bits64 ? 16 : 8);

		//Lengths (SizeT will constrain the maximum size anyways)
		SizeT bytes = data.size();
		SizeT bits = bytes * 8;

		//Helper variables
		SizeT rem = bytes % blockSize;
		SizeT lbStart = bytes / blockSize * blockSize;

		//Whether prevLastBlock will contain the second last block
		construct.prevBlockUsed = rem >= padOffset;
		
		//Total block count
		construct.blocks = bytes / blockSize + construct.prevBlockUsed + 1;

		//Fill last blocks
		if(construct.prevBlockUsed) {

			std::copy_n(data.data() + lbStart, rem, construct.prevLastBlock);
			construct.prevLastBlock[rem] = 0x80;
			
			std::fill(construct.prevLastBlock + rem + 1, construct.prevLastBlock + blockSize, u8(0));
			std::fill_n(construct.lastBlock, padOffset, 0);

		} else {

			std::copy_n(data.data() + lbStart, rem, construct.lastBlock);
			construct.lastBlock[rem] = 0x80;

			std::fill(construct.lastBlock + rem + 1, construct.lastBlock + padOffset, u8(0));

		}

		//Conditionally convert
		if (Bits::requiresEndianConversion(order)) {
			bits = Bits::swap(bits);
		}

		//Add length
		if(bits64) {

			if constexpr (sizeof(SizeT) > 8) {
				Bits::disassemble<u64>(bits >> 64, construct.lastBlock + padOffset);
			} else {
				Bits::disassemble<u64>(0, construct.lastBlock + padOffset);
			}

			Bits::disassemble<u64>(bits & ~u64(0), construct.lastBlock + padOffset + 8);

		} else {

			Bits::disassemble<u64>(bits, construct.lastBlock + padOffset);

		}

	}

}