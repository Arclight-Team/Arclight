/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 sha0.hpp
 */

#pragma once

#include "hash.hpp"
#include "common.hpp"
#include "util/bits.hpp"

#include <span>



namespace __SHA01Detail {

	template<bool SHA1>
	constexpr void dispatchBlockSHA01(const std::span<const u8>& data, u32& a0, u32& b0, u32& c0, u32& d0, u32& e0) {

		u32 w[80];

		for(u32 i = 0; i < 16; i++) {
			w[i] = Bits::big32(Bits::assemble<u32>(data.data() + i * 4));
		}

		for(u32 i = 16; i < 32; i++) {

			u32 x = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];

			//SHA1 adds this left rotation
			if constexpr (SHA1) {
				x = Bits::rol(x, 1);
			}

			w[i] = x;

		}

		//Allows better SIMD optimizations
		for(u32 i = 32; i < 80; i++) {

			u32 x = w[i - 6] ^ w[i - 16] ^ w[i - 28] ^ w[i - 32];

			//SHA1 adds this left rotation
			if constexpr (SHA1) {
				x = Bits::rol(x, 2);
			}

			w[i] = x;

		}

		u32 a = a0;
		u32 b = b0;
		u32 c = c0;
		u32 d = d0;
		u32 e = e0;

		u32 f, g;

		for(u32 i = 0; i < 80; i++) {

			if (i < 20) {

				f = (b & c) | (~b & d);
				g = 0x5A827999;

			} else if (i < 40) {

				f = b ^ c ^ d;
				g = 0x6ED9EBA1;

			} else if (i < 60) {

				f = (b & c) | (b & d) | (c & d);
				g = 0x8F1BBCDC;

			} else {

				f = b ^ c ^ d;
				g = 0xCA62C1D6;

			}

			u32 x = Bits::rol(a, 5) + e + f + g + w[i];
			e = d;
			d = c;
			c = Bits::rol(b, 30);
			b = a;
			a = x;

		}

		a0 += a;
		b0 += b;
		c0 += c;
		d0 += d;
		e0 += e;

	}


	template<bool SHA1>
	constexpr Hash<160> hashSHA01(const std::span<const u8>& data) noexcept {

		Crypto::MDConstruction<64> construct;
		Crypto::mdConstruct(construct, data, ByteOrder::Big);

		u32 a = 0x67452301;
		u32 b = 0xEFCDAB89;
		u32 c = 0x98BADCFE;
		u32 d = 0x10325476;
		u32 e = 0xC3D2E1F0;

		SizeT blocks = construct.blocks;
		u32 specialBlocks = construct.prevBlockUsed + 1;

		//Hash blocks
		if (blocks >= specialBlocks) {

			for(SizeT i = 0; i < blocks - specialBlocks; i++) {
				dispatchBlockSHA01<SHA1>(data.subspan(i * 64, 64), a, b, c, d, e);
			}

		}

		if (construct.prevBlockUsed) {
			dispatchBlockSHA01<SHA1>({construct.prevLastBlock, 64}, a, b, c, d, e);
		}

		dispatchBlockSHA01<SHA1>({construct.lastBlock, 64}, a, b, c, d, e);

		return Hash<160>(Bits::big32(a), Bits::big32(b), Bits::big32(c), Bits::big32(d), Bits::big32(e));

	}

}



namespace SHA0 {

	constexpr Hash<160> hash(const std::span<const u8>& data) noexcept {
		return __SHA01Detail::hashSHA01<false>(data);
	}

}

namespace SHA1 {

	constexpr Hash<160> hash(const std::span<const u8>& data) noexcept {
	    return __SHA01Detail::hashSHA01<true>(data);
	}

}