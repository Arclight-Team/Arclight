/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 sha2.hpp
 */

#pragma once

#include "hash.hpp"
#include "common.hpp"
#include "util/bits.hpp"
#include "util/bool.hpp"
#include "common/typetraits.hpp"

#include <span>
#include <algorithm>



namespace SHA2 {

	namespace __Detail {

		enum class SHA2Variant {
			SHA224,
			SHA256,
			SHA384,
			SHA512,
			SHA512t224,
			SHA512t256
		};

		consteval static bool is64BitSHA2Variant(SHA2Variant variant) noexcept {
			return Bool::none(variant, SHA2Variant::SHA224, SHA2Variant::SHA256);
		}

		constexpr static u32 sha2Constants32[64] = {
				0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
				0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
				0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
				0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
				0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
				0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
				0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
				0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2
		};

		constexpr static u64 sha2Constants64[80] = {
				0x428A2F98D728AE22, 0x7137449123EF65CD, 0xB5C0FBCFEC4D3B2F, 0xE9B5DBA58189DBBC, 0x3956C25BF348B538,
				0x59F111F1B605D019, 0x923F82A4AF194F9B, 0xAB1C5ED5DA6D8118, 0xD807AA98A3030242, 0x12835B0145706FBE,
				0x243185BE4EE4B28C, 0x550C7DC3D5FFB4E2, 0x72BE5D74F27B896F, 0x80DEB1FE3B1696B1, 0x9BDC06A725C71235,
				0xC19BF174CF692694, 0xE49B69C19EF14AD2, 0xEFBE4786384F25E3, 0x0FC19DC68B8CD5B5, 0x240CA1CC77AC9C65,
				0x2DE92C6F592B0275, 0x4A7484AA6EA6E483, 0x5CB0A9DCBD41FBD4, 0x76F988DA831153B5, 0x983E5152EE66DFAB,
				0xA831C66D2DB43210, 0xB00327C898FB213F, 0xBF597FC7BEEF0EE4, 0xC6E00BF33DA88FC2, 0xD5A79147930AA725,
				0x06CA6351E003826F, 0x142929670A0E6E70, 0x27B70A8546D22FFC, 0x2E1B21385C26C926, 0x4D2C6DFC5AC42AED,
				0x53380D139D95B3DF, 0x650A73548BAF63DE, 0x766A0ABB3C77B2A8, 0x81C2C92E47EDAEE6, 0x92722C851482353B,
				0xA2BFE8A14CF10364, 0xA81A664BBC423001, 0xC24B8B70D0F89791, 0xC76C51A30654BE30, 0xD192E819D6EF5218,
				0xD69906245565A910, 0xF40E35855771202A, 0x106AA07032BBD1B8, 0x19A4C116B8D2D0C8, 0x1E376C085141AB53,
				0x2748774CDF8EEB99, 0x34B0BCB5E19B48A8, 0x391C0CB3C5C95A63, 0x4ED8AA4AE3418ACB, 0x5B9CCA4F7763E373,
				0x682E6FF3D6B2B8A3, 0x748F82EE5DEFB2FC, 0x78A5636F43172F60, 0x84C87814A1F0AB72, 0x8CC702081A6439EC,
				0x90BEFFFA23631E28, 0xA4506CEBDE82BDE9, 0xBEF9A3F7B2C67915, 0xC67178F2E372532B, 0xCA273ECEEA26619C,
				0xD186B8C721C0C207, 0xEADA7DD6CDE0EB1E, 0xF57D4F7FEE6ED178, 0x06F067AA72176FBA, 0x0A637DC5A2C898A6,
				0x113F9804BEF90DAE, 0x1B710B35131C471B, 0x28DB77F523047D84, 0x32CAAB7B40C72493, 0x3C9EBE0A15C9BEBC,
				0x431D67C49C100D4C, 0x4CC5D4BECB3E42B6, 0x597F299CFC657E2A, 0x5FCB6FAB3AD6FAEC, 0x6C44198C4A475817
		};


		template<SHA2Variant Variant, class ValueT = TT::Conditional<is64BitSHA2Variant(Variant), u64, u32>>
		constexpr static void dispatchBlockSHA2(const std::span<const u8>& data, ValueT h0[8]) {

			constexpr bool bits64 = is64BitSHA2Variant(Variant);
			constexpr u32 rounds = bits64 ? 80 : 64;

			ValueT h[8];
			ValueT w[rounds];

			//Message schedule array population
			for(u32 i = 0; i < 16; i++) {
				w[i] = Bits::big(Bits::assemble<ValueT>(data.data() + i * sizeof(ValueT)));
			}

			for(u32 i = 16; i < rounds; i++) {

				if constexpr (bits64) {
					w[i] = w[i - 16] + w[i - 7] + (Bits::ror(w[i - 15], 1) ^ Bits::ror(w[i - 15], 8) ^ (w[i - 15] >> 7)) + (Bits::ror(w[i - 2], 19) ^ Bits::ror(w[i - 2], 61) ^ (w[i - 2] >> 6));
				} else {
					w[i] = w[i - 16] + w[i - 7] + (Bits::ror(w[i - 15], 7) ^ Bits::ror(w[i - 15], 18) ^ (w[i - 15] >> 3)) + (Bits::ror(w[i - 2], 17) ^ Bits::ror(w[i - 2], 19) ^ (w[i - 2] >> 10));
				}

			}

			//Copy input state
			std::copy_n(h0, 8, h);

			//Compress block
			for(u32 i = 0; i < rounds; i++) {

				ValueT a, d;

				if constexpr (bits64) {
					a = Bits::ror(h[4], 14) ^ Bits::ror(h[4], 18) ^ Bits::ror(h[4], 41);
				} else {
					a = Bits::ror(h[4], 6) ^ Bits::ror(h[4], 11) ^ Bits::ror(h[4], 25);
				}

				ValueT b = (h[4] & h[5]) ^ (~h[4] & h[6]);
				ValueT c = a + b + h[7] + w[i];

				if constexpr (bits64) {

					c += sha2Constants64[i];
					d = Bits::ror(h[0], 28) ^ Bits::ror(h[0], 34) ^ Bits::ror(h[0], 39);

				} else {

					c += sha2Constants32[i];
					d = Bits::ror(h[0], 2) ^ Bits::ror(h[0], 13) ^ Bits::ror(h[0], 22);

				}

				ValueT e = (h[0] & h[1]) ^ (h[0] & h[2]) ^ (h[1] & h[2]);
				ValueT f = d + e;

				h[7] = h[6];
				h[6] = h[5];
				h[5] = h[4];
				h[4] = h[3] + c;
				h[3] = h[2];
				h[2] = h[1];
				h[1] = h[0];
				h[0] = c + f;

			}

			//Calculate output state
			for(u32 i = 0; i < 8; i++) {
				h0[i] += h[i];
			}

		}


		template<SHA2Variant Variant>
		constexpr static auto hashSHA2(const std::span<const u8>& data) {

			constexpr bool bits64 = is64BitSHA2Variant(Variant);
			constexpr u32 bytes = bits64 ? 128 : 64;

			using ValueT = TT::Conditional<bits64, u64, u32>;

			Crypto::MDConstruction<bytes> construct {};
			Crypto::mdConstruct(construct, data, ByteOrder::Big);

			ValueT h[8];

			if constexpr (Variant == SHA2Variant::SHA224) {

				constexpr u32 s[8] = {0xC1059ED8, 0x367CD507, 0x3070DD17, 0xF70E5939, 0xFFC00B31, 0x68581511, 0x64F98FA7, 0xBEFA4FA4};
				std::copy_n(s, 8, h);

			} else if constexpr (Variant == SHA2Variant::SHA256) {

				constexpr u32 s[8] = {0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19};
				std::copy_n(s, 8, h);

			} else if constexpr (Variant == SHA2Variant::SHA384) {

				constexpr u64 s[8] = {0xCBBB9D5DC1059ED8, 0x629A292A367CD507, 0x9159015A3070DD17, 0x152FECD8F70E5939, 0x67332667FFC00B31, 0x8EB44A8768581511, 0xDB0C2E0D64F98FA7, 0x47B5481DBEFA4FA4};
				std::copy_n(s, 8, h);

			} else if constexpr (Variant == SHA2Variant::SHA512) {

				constexpr u64 s[8] = {0x6A09E667F3BCC908, 0xBB67AE8584CAA73B, 0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1, 0x510E527FADE682D1, 0x9B05688C2B3E6C1F, 0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179};
				std::copy_n(s, 8, h);

			} else if constexpr (Variant == SHA2Variant::SHA512t224) {

				constexpr u64 s[8] = {0x8C3D37C819544DA2, 0x73E1996689DCD4D6, 0x1DFAB7AE32FF9C82, 0x679DD514582F9FCF, 0x0F6D2B697BD44DA8, 0x77E36F7304C48942, 0x3F9D85A86A1D36C8, 0x1112E6AD91D692A1};
				std::copy_n(s, 8, h);

			} else if constexpr (Variant == SHA2Variant::SHA512t256) {

				constexpr u64 s[8] = {0x22312194FC2BF72C, 0x9F555FA3C84C64C2, 0x2393B86B6F53B151, 0x963877195940EABD, 0x96283EE2A88EFFE3, 0xBE5E1E2553863992, 0x2B0199FC2C85B8AA, 0x0EB72DDC81C52CA2};
				std::copy_n(s, 8, h);

			}

			SizeT blocks = construct.blocks;
			SizeT specialBlocks = construct.prevBlockUsed + 1;

			if (blocks >= specialBlocks) {

				for(SizeT i = 0; i < blocks - specialBlocks; i++) {
					dispatchBlockSHA2<Variant>(data.subspan(i * bytes, bytes), h);
				}

			}

			if (construct.prevBlockUsed) {
				dispatchBlockSHA2<Variant>({construct.prevLastBlock, bytes}, h);
			}

			dispatchBlockSHA2<Variant>({construct.lastBlock, bytes}, h);

			std::for_each_n(h, 8, [](ValueT& x) {
				x = Bits::big(x);
			});

			if constexpr (Variant == SHA2Variant::SHA224) {
				return Hash<224>(h[0], h[1], h[2], h[3], h[4], h[5], h[6]);
			} else if constexpr (Variant == SHA2Variant::SHA256) {
				return Hash<256>(h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7]);
			} else if constexpr (Variant == SHA2Variant::SHA384) {
				return Hash<384>(h[0], h[1], h[2], h[3], h[4], h[5]);
			} else if constexpr (Variant == SHA2Variant::SHA512) {
				return Hash<512>(h[0], h[1], h[2], h[3], h[4], h[5], h[6], h[7]);
			} else if constexpr (Variant == SHA2Variant::SHA512t224) {
				return Hash<224>(h[0], h[1], h[2], static_cast<u32>(h[3] & ~u32(0)));
			} else if constexpr (Variant == SHA2Variant::SHA512t256) {
				return Hash<256>(h[0], h[1], h[2], h[3]);
			}

		}

	}



	constexpr Hash<224> hash224(const std::span<const u8>& data) {
		return __Detail::hashSHA2<__Detail::SHA2Variant::SHA224>(data);
	}

	constexpr Hash<256> hash256(const std::span<const u8>& data) {
		return __Detail::hashSHA2<__Detail::SHA2Variant::SHA256>(data);
	}

	constexpr Hash<384> hash384(const std::span<const u8>& data) {
		return __Detail::hashSHA2<__Detail::SHA2Variant::SHA384>(data);
	}

	constexpr Hash<512> hash512(const std::span<const u8>& data) {
		return __Detail::hashSHA2<__Detail::SHA2Variant::SHA512>(data);
	}

	constexpr Hash<224> hash512t224(const std::span<const u8>& data) {
		return __Detail::hashSHA2<__Detail::SHA2Variant::SHA512t224>(data);
	}

	constexpr Hash<256> hash512t256(const std::span<const u8>& data) {
		return __Detail::hashSHA2<__Detail::SHA2Variant::SHA512t256>(data);
	}

}