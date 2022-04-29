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
		constexpr u16 DAC = 0xFFCC;
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

	struct ArithmeticStateTransition {

		constexpr ArithmeticStateTransition() noexcept : ArithmeticStateTransition(0, 0, 0, false) {}
		constexpr ArithmeticStateTransition(u16 lpsEst, u32 nextLPS, u32 nextMPS, bool exchangePS) noexcept : lpsEstimate(lpsEst), nextLPS(nextLPS), nextMPS(nextMPS), exchange(exchangePS) {}

		u16 lpsEstimate;
		u32 nextLPS;
		u32 nextMPS;
		bool exchange;

	};

	constexpr ArithmeticStateTransition arithmeticTransitionTable[113] = {
		{ 0x5A1D,   1,   1,  true },
		{ 0x2586,  14,   2, false },
		{ 0x1114,  16,   3, false },
		{ 0x080B,  18,   4, false },
		{ 0x03D8,  20,   5, false },
		{ 0x01DA,  23,   6, false },
		{ 0x00E5,  25,   7, false },
		{ 0x006F,  28,   8, false },
		{ 0x0036,  30,   9, false },
		{ 0x001A,  33,  10, false },
		{ 0x000D,  35,  11, false },
		{ 0x0006,   9,  12, false },
		{ 0x0003,  10,  13, false },
		{ 0x0001,  12,  13, false },
		{ 0x5A7F,  15,  15,  true },
		{ 0x3F25,  36,  16, false },
		{ 0x2CF2,  38,  17, false },
		{ 0x207C,  39,  18, false },
		{ 0x17B9,  40,  19, false },
		{ 0x1182,  42,  20, false },
		{ 0x0CEF,  43,  21, false },
		{ 0x09A1,  45,  22, false },
		{ 0x072F,  46,  23, false },
		{ 0x055C,  48,  24, false },
		{ 0x0406,  49,  25, false },
		{ 0x0303,  51,  26, false },
		{ 0x0240,  52,  27, false },
		{ 0x01B1,  54,  28, false },
		{ 0x0144,  56,  29, false },
		{ 0x00F5,  57,  30, false },
		{ 0x00B7,  59,  31, false },
		{ 0x008A,  60,  32, false },
		{ 0x0068,  62,  33, false },
		{ 0x004E,  63,  34, false },
		{ 0x003B,  32,  35, false },
		{ 0x002C,  33,   9, false },
		{ 0x5AE1,  37,  37,  true },
		{ 0x484C,  64,  38, false },
		{ 0x3A0D,  65,  39, false },
		{ 0x2EF1,  67,  40, false },
		{ 0x261F,  68,  41, false },
		{ 0x1F33,  69,  42, false },
		{ 0x19A8,  70,  43, false },
		{ 0x1518,  72,  44, false },
		{ 0x1177,  73,  45, false },
		{ 0x0E74,  74,  46, false },
		{ 0x0BFB,  75,  47, false },
		{ 0x09F8,  77,  48, false },
		{ 0x0861,  78,  49, false },
		{ 0x0706,  79,  50, false },
		{ 0x05CD,  48,  51, false },
		{ 0x04DE,  50,  52, false },
		{ 0x040F,  50,  53, false },
		{ 0x0363,  51,  54, false },
		{ 0x02D4,  52,  55, false },
		{ 0x025C,  53,  56, false },
		{ 0x01F8,  54,  57, false },
		{ 0x01A4,  55,  58, false },
		{ 0x0160,  56,  59, false },
		{ 0x0125,  57,  60, false },
		{ 0x00F6,  58,  61, false },
		{ 0x00CB,  59,  62, false },
		{ 0x00AB,  61,  63, false },
		{ 0x008F,  61,  32, false },
		{ 0x5B12,  65,  65,  true },
		{ 0x4D04,  80,  66, false },
		{ 0x412C,  81,  67, false },
		{ 0x37D8,  82,  68, false },
		{ 0x2FE8,  83,  69, false },
		{ 0x293C,  84,  70, false },
		{ 0x2379,  86,  71, false },
		{ 0x1EDF,  87,  72, false },
		{ 0x1AA9,  87,  73, false },
		{ 0x174E,  72,  74, false },
		{ 0x1424,  72,  75, false },
		{ 0x119C,  74,  76, false },
		{ 0x0F6B,  74,  77, false },
		{ 0x0D51,  75,  78, false },
		{ 0x0BB6,  77,  79, false },
		{ 0x0A40,  77,  48, false },
		{ 0x5832,  80,  81,  true },
		{ 0x4D1C,  88,  82, false },
		{ 0x438E,  89,  83, false },
		{ 0x3BDD,  90,  84, false },
		{ 0x34EE,  91,  85, false },
		{ 0x2EAE,  92,  86, false },
		{ 0x299A,  93,  87, false },
		{ 0x2516,  86,  71, false },
		{ 0x5570,  88,  89,  true },
		{ 0x4CA9,  95,  90, false },
		{ 0x44D9,  96,  91, false },
		{ 0x3E22,  97,  92, false },
		{ 0x3824,  99,  93, false },
		{ 0x32B4,  99,  94, false },
		{ 0x2E17,  93,  86, false },
		{ 0x56A8,  95,  96,  true },
		{ 0x4F46, 101,  97, false },
		{ 0x47E5, 102,  98, false },
		{ 0x41CF, 103,  99, false },
		{ 0x3C3D, 104, 100, false },
		{ 0x375E,  99,  93, false },
		{ 0x5231, 105, 102, false },
		{ 0x4C0F, 106, 103, false },
		{ 0x4639, 107, 104, false },
		{ 0x415E, 103,  99, false },
		{ 0x5627, 105, 106,  true },
		{ 0x50E7, 108, 107, false },
		{ 0x4B85, 109, 103, false },
		{ 0x5597, 110, 109, false },
		{ 0x504F, 111, 107, false },
		{ 0x5A10, 110, 111,  true },
		{ 0x5522, 112, 109, false },
		{ 0x59EB, 112, 111,  true }
	};

	constexpr u8 dezigzagTable[64] = {
		 0,  1,  8, 16,  9,  2,  3, 10,
		17, 24, 32, 25, 18, 11,  4,  5,
		12, 19, 26, 33, 40, 48, 41, 34,
		27, 20, 13,  6,  7, 14, 21, 28,
		35, 42, 49, 56, 57, 50, 43, 36,
		29, 22, 15, 23, 30, 37, 44, 51,
		58, 59, 52, 45, 38, 31, 39, 46,
		53, 60, 61, 54, 47, 55, 62, 63
	};

	constexpr u8 dezigzagTableTransposed[64] = {
		 0,  8,  1,  2,  9, 16, 24, 17,
		10,  3,  4, 11, 18, 25, 32, 40,
		33, 26, 19, 12,  5,  6, 13, 20,
		27, 34, 41, 48, 56, 49, 42, 35,
		28, 21, 14,  7, 15, 22, 29, 36,
		43, 50, 57, 58, 51, 44, 37, 30,
		23, 31, 38, 45, 52, 59, 60, 53,
		46, 39, 47, 54, 61, 62, 55, 63
	};

	constexpr u8 zigzagIndexTransposed[64] = {
		 0,  2,  3,  9, 10, 20, 21, 35,
		 1,  4,  8, 11, 19, 22, 34, 36,
		 5,  7, 12, 18, 23, 33, 37, 48,
		 6, 13, 17, 24, 32, 38, 47, 49,
		14, 16, 25, 31, 39, 46, 50, 57,
		15, 26, 30, 40, 45, 51, 56, 58,
		27, 29, 41, 44, 52, 55, 59, 62,
		28, 42, 43, 53, 54, 60, 61, 63
	};

	constexpr u8 jfifString[5] = {0x4A, 0x46, 0x49, 0x46, 0x00};
	constexpr u8 jfxxString[5] = {0x4A, 0x46, 0x58, 0x58, 0x00};


	struct QuantizationTable {

		std::array<i32, 64> data;
		bool hasData;

		QuantizationTable() { reset(); }

		constexpr bool empty() const noexcept {
			return !hasData;
		}

		void reset() {

			std::fill(data.begin(), data.end(), 0);
			hasData = false;

		}

	};


	using HuffmanResult = std::pair<u8, u8>;

	struct HuffmanTable {

		constexpr static HuffmanResult defaultHuffmanResult = {0xC, 0x1};

		HuffmanTable() { reset(); }

		constexpr bool empty() const noexcept {
			return !maxLength;
		}

		void reset() {

			maxLength = 0;
			fastTable.fill(defaultHuffmanResult);
			extTables.clear();

		}

		u32 maxLength;
		std::array<HuffmanResult, 256> fastTable;
		std::vector<std::vector<HuffmanResult>> extTables;

	};

	struct ArithmeticDCConditioning {

		constexpr ArithmeticDCConditioning() noexcept : lowerBound(0), upperBound(0), active(false) {}

		i32 lowerBound;
		i32 upperBound;
		bool active;

	};

	struct ArithmeticACConditioning {

		constexpr ArithmeticACConditioning() noexcept : kx(1), active(false) {}

		u8 kx;
		bool active;

	};

	struct FrameComponent {

		constexpr FrameComponent() noexcept : FrameComponent(0, 0, 0) {}
		constexpr FrameComponent(u32 sx, u32 sy, u32 qTableID) noexcept : samplesX(sx), samplesY(sy), qID(qTableID), width(0), height(0), progression(0) {}

		u32 samplesX, samplesY;
		u32 qID;

		u32 width, height;

		u32 progression;
		std::vector<i32> progressiveBuffer;
		std::vector<i16> imageData;


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

	struct ScanComponent {

		constexpr ScanComponent(HuffmanTable& dct, HuffmanTable& act, ArithmeticDCConditioning& dcc, ArithmeticACConditioning& acc, QuantizationTable& qt, FrameComponent& component)
			: dcTable(dct), acTable(act), dcConditioning(dcc), acConditioning(acc), qTable(qt), frameComponent(component), prediction(0), prevDifference(0), block(nullptr) {}

		HuffmanTable& dcTable;
		HuffmanTable& acTable;
		ArithmeticDCConditioning& dcConditioning;
		ArithmeticACConditioning& acConditioning;
		QuantizationTable& qTable;
		FrameComponent& frameComponent;

		i32 prediction;
		i32 prevDifference;
		i32* block;

	};

	struct Scan {

		constexpr Scan() noexcept : spectralStart(0), spectralEnd(0), approximationHigh(0), approximationLow(0), maxSamplesX(0), maxSamplesY(0), mcuDataUnits(0), totalMCUs(0), mcusX(0), mcusY(0) {}

		std::vector<ScanComponent> scanComponents;
		u32 spectralStart;
		u32 spectralEnd;
		u32 approximationHigh;
		u32 approximationLow;

		u32 maxSamplesX, maxSamplesY;
		u32 mcuDataUnits;
		u32 mcusX, mcusY;
		u32 totalMCUs;

	};

	struct Progression {



	};

}