/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 jpeg.hpp
 */

#pragma once

#include "memory/alignedallocator.hpp"
#include "types.hpp"

#include <array>
#include <vector>
#include <unordered_map>



namespace JPEG {

	namespace Markers {

		constexpr u16 SOF0 = 0xFFC0;
		constexpr u16 SOF1 = 0xFFC1;
		constexpr u16 SOF2 = 0xFFC2;
		constexpr u16 SOF3 = 0xFFC3;
		constexpr u16 DHT = 0xFFC4;
		constexpr u16 SOF5 = 0xFFC5;
		constexpr u16 SOF6 = 0xFFC6;
		constexpr u16 SOF7 = 0xFFC7;
		constexpr u16 JPG = 0xFFC8;
		constexpr u16 SOF9 = 0xFFC9;
		constexpr u16 SOF10 = 0xFFCA;
		constexpr u16 SOF11 = 0xFFCB;
		constexpr u16 SOF13 = 0xFFCD;
		constexpr u16 SOF14 = 0xFFCE;
		constexpr u16 SOF15 = 0xFFCF;
		constexpr u16 SOI = 0xFFD8;
		constexpr u16 EOI = 0xFFD9;
		constexpr u16 SOS = 0xFFDA;
		constexpr u16 DQT = 0xFFDB;
		constexpr u16 DRI = 0xFFDD;
		constexpr u16 APP0 = 0xFFE0;
		constexpr u16 APP1 = 0xFFE1;

	};

	enum class FrameType {
		Baseline,
		ExtendedSequential,
		Progressive,
		Lossless
	};

	enum class Encoding {
		Huffman,
		Arithmetic
	};

	constexpr u32 dezigzagTable[64] = {
		 0,  1,  8, 16,  9,  2,  3, 10,
		17, 24, 32, 25, 18, 11,  4,  5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13,  6,  7, 14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63
	};

	constexpr u32 dezigzagTableTransposed[64] = {
		 0,  8,  1,  2,  9, 16, 24, 17,
		10,  3,  4, 11, 18, 25, 32, 40,
		33, 26, 19, 12,  5,  6, 13, 20,
		27, 34, 41, 48, 56, 49, 42, 35,
		28, 21, 14,  7, 15, 22, 29, 36,
		43, 50, 57, 58, 51, 44, 37, 30,
		23, 31, 38, 45, 52, 59, 60, 53,
		46, 39, 47, 54, 61, 62, 55, 63
	};

	constexpr u8 jfifString[5] = {0x4A, 0x46, 0x49, 0x46, 0x00};
	constexpr u8 jfxxString[5] = {0x4A, 0x46, 0x58, 0x58, 0x00};

	using QuantizationTable = std::array<i32, 64>;
	using HuffmanTable = std::vector<std::pair<u8, u8>>;

	struct FrameComponent {

		constexpr FrameComponent() noexcept : FrameComponent(0, 0, 0) {}
		constexpr FrameComponent(u8 sx, u8 sy, u8 q) noexcept : sx(sx), sy(sy), qID(q) {}

		u8 sx, sy, qID;

	};

	struct Frame {

		Frame() : type(FrameType::Baseline), differential(false), encoding(Encoding::Huffman), bits(8), lines(0), samples(1) {}

		FrameType type;
		bool differential;
		Encoding encoding;

		u32 bits;
		u32 lines;
		u32 samples;

		std::unordered_map<u8, FrameComponent> components;

	};

	struct ImageComponent {

		constexpr ImageComponent(HuffmanTable& dct, HuffmanTable& act, QuantizationTable& qt, u32 sx, u32 sy)
			: dcTable(dct), acTable(act), qTable(qt), samplesX(sx), samplesY(sy), prediction(0), width(0), height(0), dcLength(0), acLength(0), dataUnit(0) {}

		HuffmanTable& dcTable;
		HuffmanTable& acTable;
		QuantizationTable& qTable;

		u32 samplesX, samplesY;
		u32 width, height;

		i32 prediction;
		u32 dcLength, acLength;
		u32 dataUnit;
		std::vector<i32, AlignedAllocator<i32, 16>> blockData;
		std::vector<i32, AlignedAllocator<i32, 16>> imageData;

	};

	struct Scan {

		constexpr Scan() noexcept : spectralStart(0), spectralEnd(0), approximationHigh(0), approximationLow(0), maxSamplesX(0), maxSamplesY(0), mcuDataUnits(0), totalMCUs(0), mcusX(0), mcusY(0) {}

		std::vector<ImageComponent> imageComponents;
		u32 spectralStart;
		u32 spectralEnd;
		u32 approximationHigh;
		u32 approximationLow;

		u32 maxSamplesX, maxSamplesY;
		u32 mcuDataUnits;
		u32 mcusX, mcusY;
		u32 totalMCUs;

	};

}