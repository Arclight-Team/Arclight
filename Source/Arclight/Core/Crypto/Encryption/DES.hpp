/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 DES.hpp
 */

#pragma once

#include "Key.hpp"
#include "Meta/TypeTraits.hpp"
#include "Util/Bits.hpp"
#include "Common/Assert.hpp"
#include "Util/Bool.hpp"

#include <span>
#include <array>



namespace DES {

	namespace __Detail {

		// Permuted Choice 1
		constexpr u8 pc1[56] =
		{
			57, 49, 41, 33, 25, 17,  9,
			 1, 58, 50, 42, 34, 26, 18,
			10,  2, 59, 51, 43, 35, 27,
			19, 11,  3, 60, 52, 44, 36,
			63, 55, 47, 39, 31, 23, 15,
			 7, 62, 54, 46, 38, 30, 22,
			14,  6, 61, 53, 45, 37, 29,
			21, 13,  5, 28, 20, 12,  4
		};

		// Permuted Choice 2
		constexpr u8 pc2[48] =
		{
			14, 17, 11, 24,  1,  5,
			 3, 28, 15,  6, 21, 10,
			23, 19, 12,  4, 26,  8,
			16,  7, 27, 20, 13,  2,
			41, 52, 31, 37, 47, 55,
			30, 40, 51, 45, 33, 48,
			44, 49, 39, 56, 34, 53,
			46, 42, 50, 36, 29, 32
		};

		// Expansion
		constexpr u8 e[48] =
		{
			32,  1,  2,  3,  4,  5,
			 4,  5,  6,  7,  8,  9,
			 8,  9, 10, 11, 12, 13,
			12, 13, 14, 15, 16, 17,
			16, 17, 18, 19, 20, 21,
			20, 21, 22, 23, 24, 25,
			24, 25, 26, 27, 28, 29,
			28, 29, 30, 31, 32,  1
		};

		// Substitution Boxes
		constexpr u8 s[8][64] =
		{
			14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
			 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
			 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
			15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
			15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
			 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
			 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
			13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
			10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
			13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
			13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
			 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
			 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
			13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
			10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
			 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
			 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
			14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
			 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
			11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
			12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
			10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
			 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
			 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
			 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
			13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
			 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
			 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
			13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
			 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
			 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
			 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11,
		};

		// Initial Permutation
		constexpr u8 ip[64] =
		{
			58, 50, 42, 34, 26, 18, 10,  2,
			60, 52, 44, 36, 28, 20, 12,  4,
			62, 54, 46, 38, 30, 22, 14,  6,
			64, 56, 48, 40, 32, 24, 16,  8,
			57, 49, 41, 33, 25, 17,  9,  1,
			59, 51, 43, 35, 27, 19, 11,  3,
			61, 53, 45, 37, 29, 21, 13,  5,
			63, 55, 47, 39, 31, 23, 15,  7
		};

		// Permutation
		constexpr u8 p[32] =
		{
			16,  7, 20, 21,
			29, 12, 28, 17,
			 1, 15, 23, 26,
			 5, 18, 31, 10,
			 2,  8, 24, 14,
			32, 27,  3,  9,
			19, 13, 30,  6,
			22, 11,  4, 25
		};
		
		// Final Permutation
		constexpr u8 p1[64] =
		{
			40,  8, 48, 16, 56, 24, 64, 32,
			39,  7, 47, 15, 55, 23, 63, 31,
			38,  6, 46, 14, 54, 22, 62, 30,
			37,  5, 45, 13, 53, 21, 61, 29,
			36,  4, 44, 12, 52, 20, 60, 28,
			35,  3, 43, 11, 51, 19, 59, 27,
			34,  2, 42, 10, 50, 18, 58, 26,
			33,  1, 41,  9, 49, 17, 57, 25
		};

		
		constexpr u64 readBlock(const std::span<const u8>& bytes, u64 position) {

			SizeT remaining = bytes.size() - position;

			return Bits::assemble<u64>(bytes.data(), Math::min(remaining, 8));

		}

		constexpr CryptoKey<56> permuteKey(const CryptoKey<64>& key) {

			CryptoKey<56> out;

			// Permute and shrink key to 56 bits
			for (u32 i = 0; i < 56; i++) {
				out[55 - i] = key[64 - pc1[i]];
			}

			return out;

		}

		template<bool Upper>
		constexpr u32 splitKey(const CryptoKey<56>& key) {
			return key.subKey<Upper ? 28 : 0, 28>().bits().template read<u32>(28);
		}

		constexpr CryptoKey<48> concatKeys(u32 keyL, u32 keyR) {

			// Concatenate two 28-bit key into a 56-bit key
			CryptoKey<64> key(keyL | u64(keyR) << 28);
			CryptoKey<48> out;

			// Permute and shrink key to 48 bits
			for (u32 i = 0; i < 48; i++) {
				out[47 - i] = key[56 - pc2[i]];
			}

			return out;

		}

		constexpr u32 shiftKey(u32 v, u32 n) {

			n = n % 28;

			u32 r = (1u << 28) - 1;
			r &= v << n;
			r |= v >> (28 - n);

			return r;

		}

		template<bool Final>
		constexpr u64 permuteBlock(u64 block) {

			// Input block data
			u8 bData[8];
			Bits::disassemble(block, bData);
			auto bBits = BitSpan<64>(bData);

			// Data calculated from the permutation
			u8 pData[8]{};
			auto pBits = BitSpan<64, false>(pData);

			for (u32 i = 0; i < 64; i++) {

				if constexpr (Final) {
					pBits[63 - i] = bBits[64 - p1[i]];
				} else {
					pBits[63 - i] = bBits[64 - ip[i]];
				}

			}

			return Bits::assemble<u64>(pData);

		}

		constexpr u8 sBoxIndex(u8 bits) {

			u8 row = (bits >> 1) & 0xF;
			u8 col = (bits & 0x20) | ((bits << 4) & 0x10);

			return row | col;

		}

		constexpr u32 feistel(u32 block, const CryptoKey<48>& key) {

			// Input block data
			u8 bData[4]{};
			Bits::disassemble(block, bData);
			auto bBits = BitSpan<32>(bData);
			
			// Data calculated from E ^ key
			u8 eData[6]{};
			auto eBits = BitSpan<48, false>(eData);

			// Expand the data using the E table and XOR with the key
			for (u32 i = 0; i < 48; i++) {
				eBits[i] = bBits[e[i] - 1] ^ key[i];
			}

			// Data calculated from the S boxes
			u8 sData[4]{};
			auto sBits = BitSpan<32, false>(sData);
			
			// Calculate a 32-bit value using the S boxes
			for (u32 i = 0; i < 8; i++) {
				
				// Read 6 bits to use as the S box index
				u8 sIndex = eBits.subspan(i * 6, 6).read<u8>(6);

				// Read 4 bits from the S box
				u8 s4Bits = s[7 - i][sBoxIndex(sIndex)];

				// Write the 4 bits to the output block
				sBits.subspan(i * 4, 4).write(4, s4Bits);
				
			}

			// Data calculated from the P permutation
			u8 pData[4]{};
			auto pBits = BitSpan<32, false>(pData);

			// Permute the data using the P table
			for (u32 i = 0; i < 32; i++) {
				pBits[31 - i] = sBits[32 - p[i]];
			}

			return Bits::assemble<u32>(pData);

		}

		constexpr void process(bool decrypt, const std::span<const u8>& input, const std::span<u8>& output, const CryptoKey<64>& key) {

			// Permute input key
			CryptoKey<56> permutedKey = permuteKey(key);

			// Split 56-bit key into two 28-bit keys
			u32 keyL = splitKey<0>(permutedKey);
			u32 keyR = splitKey<1>(permutedKey);

			// Generate 16 keys
			CryptoKey<48> keys[16];

			for (u32 s = 0; s < 16; s++) {

				u8 shift = Bool::any(s, 0, 1, 8, 15) ? 1 : 2;

				// Shift the two keys to the left
				keyL = shiftKey(keyL, shift);
				keyR = shiftKey(keyR, shift);

				// Concatenate the two keys and permute the final 48-bit key
				keys[s] = concatKeys(keyL, keyR);

			}

			for (SizeT p = 0; p < input.size(); p += 8) {

				// Read block from input storage
				u64 block = readBlock(input, p);

				// Permute block using the IP table
				block = permuteBlock<0>(block);

				// Split 64-bit block into two 32-bit blocks
				u32 blockL = block >> 32;
				u32 blockR = block;

				for (u32 s = 0; s < 16; s++) {

					u32 prevL = blockL;
					u32 prevR = blockR;

					blockL = prevR;
					blockR = prevL ^ feistel(prevR, keys[decrypt ? (15 - s) : s]);

				}

				// Concatenate the two 32-bit blocks into a 64-bit block
				block = blockL | u64(blockR) << 32;

				// Permute block using the P-1 table
				block = permuteBlock<1>(block);

				// Write block to output storage
				Bits::disassemble(block, output.data() + p);

			}

		}

	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto encrypt(const std::span<const u8, Extent>& data, const CryptoKey<64>& key) {

		// Calculate size aligned to 8 bytes
		std::array<u8, Bits::alignUp(Extent, 8)> result{};
		
		// Encrypt
		__Detail::process(0, data, result, key);

		return result;

	}

	constexpr auto encrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<64>& key) {

		// Calculate size aligned to 8 bytes
		SizeT alignedSize = Bits::alignUp(data.size(), 8);

		// Allocate data buffer
		std::vector<u8> result(alignedSize);

		// Encrypt
		__Detail::process(0, data, result, key);

		return result;

	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto decrypt(const std::span<const u8, Extent>& data, const CryptoKey<64>& key) {

		// Calculate size aligned to 8 bytes
		std::array<u8, Bits::alignUp(Extent, 8)> result{};

		// Decrypt
		__Detail::process(1, data, result, key);

		return result;

	}

	constexpr auto decrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<64>& key) {

		// Calculate size aligned to 8 bytes
		SizeT alignedSize = Bits::alignUp(data.size(), 8);

		// Allocate data buffer
		std::vector<u8> result(alignedSize);

		// Decrypt
		__Detail::process(1, data, result, key);

		// Resize to fit
		result.resize(data.size());

		return result;

	}

}


namespace TripleDES {

	namespace __Detail = DES::__Detail;

	/*
	*	1 key encryption/decryption -> 56-bit key (DES)
	*/

	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto encrypt(const std::span<const u8, Extent>& data, const CryptoKey<64>& key) {
		return DES::encrypt<Extent>(data, key);
	}

	constexpr auto encrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<64>& key) {
		return DES::encrypt(data, key);
	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto decrypt(const std::span<const u8, Extent>& data, const CryptoKey<64>& key) {
		return DES::decrypt<Extent>(data, key);
	}

	constexpr auto decrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<64>& key) {
		return DES::decrypt(data, key);
	}

	/*
	*	2 keys encryption/decryption -> 112-bit key
	*/


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto encrypt(const std::span<const u8, Extent>& data, const CryptoKey<64>& key1, const CryptoKey<64>& key2) {

		// Calculate size aligned to 8 bytes
		std::array<u8, Bits::alignUp(Extent, 8)> result{};

		// Encrypt - first stage
		__Detail::process(0, data, result, key1);

		// Decrypt - second stage
		__Detail::process(1, result, result, key2);

		// Encrypt - third stage
		__Detail::process(0, result, result, key1);

		return result;

	}

	constexpr auto encrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<64>& key1, const CryptoKey<64>& key2) {

		// Calculate size aligned to 8 bytes
		SizeT alignedSize = Bits::alignUp(data.size(), 8);

		// Allocate data buffer
		std::vector<u8> result(alignedSize);

		// Encrypt - first stage
		__Detail::process(0, data, result, key1);

		// Decrypt - second stage
		__Detail::process(1, result, result, key2);

		// Encrypt - third stage
		__Detail::process(0, result, result, key1);

		return result;

	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto decrypt(const std::span<const u8, Extent>& data, const CryptoKey<64>& key1, const CryptoKey<64>& key2) {

		// Calculate size aligned to 8 bytes
		std::array<u8, Bits::alignUp(Extent, 8)> result{};

		// Decrypt - first stage
		__Detail::process(1, data, result, key1);

		// Encrypt - second stage
		__Detail::process(0, result, result, key2);

		// Decrypt - third stage
		__Detail::process(1, result, result, key1);

		return result;

	}

	constexpr auto decrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<64>& key1, const CryptoKey<64>& key2) {

		// Calculate size aligned to 8 bytes
		SizeT alignedSize = Bits::alignUp(data.size(), 8);

		// Allocate data buffer
		std::vector<u8> result(alignedSize);

		// Decrypt - first stage
		__Detail::process(1, data, result, key1);

		// Encrypt - second stage
		__Detail::process(0, result, result, key2);

		// Decrypt - third stage
		__Detail::process(1, result, result, key1);

		return result;

	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto encrypt(const std::span<const u8, Extent>& data, const CryptoKey<128>& key) {
		return encrypt<Extent>(data, key.subKey<0, 64>(), key.subKey<64, 64>());
	}

	constexpr auto encrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<128>& key) {
		return encrypt(data, key.subKey<0, 64>(), key.subKey<64, 64>());
	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto decrypt(const std::span<const u8, Extent>& data, const CryptoKey<128>& key) {
		return decrypt<Extent>(data, key.subKey<0, 64>(), key.subKey<64, 64>());
	}

	constexpr auto decrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<128>& key) {
		return decrypt(data, key.subKey<0, 64>(), key.subKey<64, 64>());
	}

	/*
	*	3 keys encryption/decryption -> 168-bit key
	*/

	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto encrypt(const std::span<const u8, Extent>& data, const CryptoKey<64>& key1, const CryptoKey<64>& key2, const CryptoKey<64>& key3) {

		// Calculate size aligned to 8 bytes
		std::array<u8, Bits::alignUp(Extent, 8)> result{};

		// Encrypt - first stage
		__Detail::process(0, data, result, key1);

		// Decrypt - second stage
		__Detail::process(1, result, result, key2);

		// Encrypt - third stage
		__Detail::process(0, result, result, key3);

		return result;

	}

	constexpr auto encrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<64>& key1, const CryptoKey<64>& key2, const CryptoKey<64>& key3) {

		// Calculate size aligned to 8 bytes
		SizeT alignedSize = Bits::alignUp(data.size(), 8);

		// Allocate data buffer
		std::vector<u8> result(alignedSize);

		// Encrypt - first stage
		__Detail::process(0, data, result, key1);

		// Decrypt - second stage
		__Detail::process(1, result, result, key2);

		// Encrypt - third stage
		__Detail::process(0, result, result, key3);

		return result;

	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto decrypt(const std::span<const u8, Extent>& data, const CryptoKey<64>& key1, const CryptoKey<64>& key2, const CryptoKey<64>& key3) {

		// Calculate size aligned to 8 bytes
		std::array<u8, Bits::alignUp(Extent, 8)> result{};

		// Decrypt - first stage
		__Detail::process(1, data, result, key1);

		// Encrypt - second stage
		__Detail::process(0, result, result, key2);

		// Decrypt - third stage
		__Detail::process(1, result, result, key3);

		return result;

	}

	constexpr auto decrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<64>& key1, const CryptoKey<64>& key2, const CryptoKey<64>& key3) {

		// Calculate size aligned to 8 bytes
		SizeT alignedSize = Bits::alignUp(data.size(), 8);

		// Allocate data buffer
		std::vector<u8> result(alignedSize);

		// Decrypt - first stage
		__Detail::process(1, data, result, key1);

		// Encrypt - second stage
		__Detail::process(0, result, result, key2);

		// Decrypt - third stage
		__Detail::process(1, result, result, key3);

		return result;

	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto encrypt(const std::span<const u8, Extent>& data, const CryptoKey<192>& key) {
		return encrypt<Extent>(data, key.subKey<0, 64>(), key.subKey<64, 64>(), key.subKey<128, 64>());
	}

	constexpr auto encrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<192>& key) {
		return encrypt(data, key.subKey<0, 64>(), key.subKey<64, 64>(), key.subKey<128, 64>());
	}


	template<SizeT Extent> requires(Extent != std::dynamic_extent)
	constexpr auto decrypt(const std::span<const u8, Extent>& data, const CryptoKey<192>& key) {
		return decrypt<Extent>(data, key.subKey<0, 64>(), key.subKey<64, 64>(), key.subKey<128, 64>());
	}

	constexpr auto decrypt(const std::span<const u8, std::dynamic_extent>& data, const CryptoKey<192>& key) {
		return decrypt(data, key.subKey<0, 64>(), key.subKey<64, 64>(), key.subKey<128, 64>());
	}

}
