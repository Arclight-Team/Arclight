/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 charactermap.cpp
 */

#include "font/truetype/truetype.hpp"
#include "stream/binaryreader.hpp"
#include "util/bool.hpp"



namespace TrueType {


	std::unordered_map<u32, u32> parseSubtableVersion4(BinaryReader& reader, u32 tableSize) {

		if(tableSize < 12) {
			throw LoaderException("Failed to load character map subtable 4: Stream size too small");
		}

		SizeT tableBase = reader.getPosition();

		u16 length = reader.read<u16>();
		u16 language = reader.read<u16>();
		u16 segCountX2 = reader.read<u16>();
		u16 searchRange = reader.read<u16>();
		u16 entrySelector = reader.read<u16>();
		u16 rangeShift = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
		Log::info("TrueType Loader", "[Character Map Subtable 4] Length: 0x%X, Language: %d, SegCountX2: %d, Range: %d, Entry: %d, Unspotted: %d",
			length, language, segCountX2, searchRange, entrySelector, rangeShift);
#endif

		u32 segmentCount = segCountX2 / 2;
		u32 reqSize = segmentCount * 8 + 2 + 12;

		if(segmentCount < 2) {
			throw LoaderException("Failed to load character map subtable 4: Table must contain at least 2 segments");
		}

		if(tableSize < reqSize) {
			throw LoaderException("Failed to load character map subtable 4: Stream size too small");
		}

		std::vector<u16> segmentStarts(segmentCount);
		std::vector<u16> segmentEnds(segmentCount);
		std::vector<u16> segmentDeltas(segmentCount);
		std::vector<u16> segmentOffsets(segmentCount);
		
		for(u32 i = 0; i < segmentCount; i++) {
			segmentEnds[i] = reader.read<u16>();
		}

		u16 reservedPad = reader.read<u16>();

		for(u32 i = 0; i < segmentCount; i++) {
			segmentStarts[i] = reader.read<u16>();
		}

		for(u32 i = 0; i < segmentCount; i++) {
			segmentDeltas[i] = reader.read<u16>();
		}

		for(u32 i = 0; i < segmentCount; i++) {
			segmentOffsets[i] = reader.read<u16>();
		}

		std::unordered_map<u32, u32> charMap;
		u32 rangeBase = segmentCount * 6 + 14;

		for(u32 i = 0; i < segmentCount; i++) {

			u32 start = segmentStarts[i];
			u32 end = segmentEnds[i];

			if(start == 0xFFFF || end == 0xFFFF) {

				if(i != segmentCount - 1) {
					throw LoaderException("Failed to load character map subtable 4: Illegal table terminator");
				}

				break;

			}

			if(start > end) {
				throw LoaderException("Failed to load character map subtable 4: Segment start cannot be greater than segment end");
			}

			//Per segment
			u32 rangeOffset = segmentOffsets[i];
			u32 delta = segmentDeltas[i];

			if(rangeOffset != 0) {

				u32 glyphIndexBaseOffset = rangeBase + rangeOffset + i * 2;

				if(tableSize < glyphIndexBaseOffset + (end - start + 1) * 2) {
					throw LoaderException("Failed to load character map subtable 4: Stream size too small");
				}

				reader.getStream().seekTo(tableBase + glyphIndexBaseOffset);

				for(u32 cp = start; cp <= end; cp++) {

					u32 glyphID = reader.read<u16>();

					if(glyphID != 0) {

						glyphID += delta;
						glyphID &= 0xFFFF;

					}

					charMap[cp] = glyphID;

				}

			} else {

				for(u32 cp = start; cp <= end; cp++) {
					charMap[cp] = (cp + delta) & 0xFFFF;
				}

			}

		}

		return charMap;

	}


	std::unordered_map<u32, u32> parseSubtableVersion12(BinaryReader& reader, u32 tableSize) {

		if(tableSize < 14) {
			throw LoaderException("Failed to load character map subtable 12: Stream size too small");
		}

		u16 reserved = reader.read<u16>();
		u32 length = reader.read<u32>();
		u32 language = reader.read<u32>();
		u32 groups = reader.read<u32>();

#ifdef ARC_FONT_DEBUG
		Log::info("TrueType Loader", "[Character Map Subtable 12] Reserved: %d, Length: 0x%X, Language: %d, Groups: %d", reserved, length, language, groups);
#endif

		if(groups * 12 + 14 < tableSize) {
			throw LoaderException("Failed to load character map subtable 12: Stream size too small");  
		}

		std::unordered_map<u32, u32> charMap;

		for(u32 i = 0; i < groups; i++) {

			u32 start = reader.read<u32>();
			u32 end = reader.read<u32>();
			u32 glyphStart = reader.read<u32>();

			for(u32 j = start; j <= end; j++) {
				charMap[j] = glyphStart++;
			}

		}

		return charMap;

	}



	std::unordered_map<u32, u32> parseCharacterMapTable(BinaryReader& reader, u32 tableSize) {

		if(tableSize < 4) {
			throw LoaderException("Failed to load character map table: Stream size too small");
		}

		SizeT startStreamPos = reader.getPosition();
		u16 version = reader.read<u16>();
		u16 numberSubtables = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
		Log::info("TrueType Loader", "[Character Map] Version: 0x%04X, NumSubtables: %d", version, numberSubtables);
#endif

		if(version != 0) {
			throw LoaderException("Failed to load character map table: Illegal character map table version"); 
		}

		if(numberSubtables == 0) {
			throw LoaderException("Failed to load character map table: Subtable count cannot be 0"); 
		}

		u32 subtableHeaderSize = numberSubtables * 8 + 4;

		if(tableSize < subtableHeaderSize) {
			throw LoaderException("Failed to load character map table: Stream size too small");
		}

		std::vector<CMapSubtableHeader> unicodeHeaders;
		std::vector<CMapSubtableHeader> uvsHeaders;
		std::vector<CMapSubtableHeader> microsoftHeaders;
		std::vector<CMapSubtableHeader> symbolHeaders;
		std::vector<CMapSubtableHeader> macintoshHeaders;

		for(u32 i = 0; i < numberSubtables; i++) {

			CMapSubtableHeader header;
			header.platformID = reader.read<u16>();
			header.platformSpecificID = reader.read<u16>();
			header.offset = reader.read<u32>();

#ifdef ARC_FONT_DEBUG
			Log::info("TrueType Loader", "[Character Map Subtable] PlatformID: %d, SpecificID: %d, Offset: 0x%08X", header.platformID, header.platformSpecificID, header.offset);
#endif

			if(!verifyPlatformID(header.platformID, header.platformSpecificID, true)) {
				throw LoaderException("Illegal platform ID/specific ID combination (PID: %d, SID: %d)", header.platformID, header.platformSpecificID);
			}

			PlatformID id = static_cast<PlatformID>(header.platformID);

			switch(id) {

				case PlatformID::Unicode:

					if(Bool::one<i32>(header.platformSpecificID, static_cast<i32>(UnicodeEncoding::UVS), 14)) {
						uvsHeaders.push_back(header);
					} else {
						unicodeHeaders.push_back(header);
					}

					break;

				case PlatformID::Macintosh:
					macintoshHeaders.push_back(header);
					break;

				case PlatformID::Microsoft:

					if(static_cast<MicrosoftEncoding>(header.platformSpecificID) == MicrosoftEncoding::Symbol) {
						symbolHeaders.push_back(header);
					} else {
						microsoftHeaders.push_back(header);
					}

					break;

				default:
					arc_force_assert("Illegal platform ID");
					macintoshHeaders.push_back(header);
					break;

			}

		}

		//Choose the best subtable for our use
		CMapSubtableHeader bestHeader;
		bool useUvs = false;

		if(uvsHeaders.size()) {
				useUvs = true;
		}

		if(symbolHeaders.size()) {

			bestHeader = symbolHeaders[0];

		} else if (unicodeHeaders.size() > uvsHeaders.size()) {

			u16 bestUnicodeEncoding = 6;
			u32 bestIndex = -1;

			constexpr static UnicodeEncoding encodingOrder[] = {
				UnicodeEncoding::Unicode2Full, UnicodeEncoding::Unicode2BMP, UnicodeEncoding::Version11, UnicodeEncoding::Version10, UnicodeEncoding::ISO10646, UnicodeEncoding::LastResort
			};

			for(u32 i = 0; i < unicodeHeaders.size(); i++) {

				UnicodeEncoding curEncoding = static_cast<UnicodeEncoding>(unicodeHeaders[i].platformSpecificID);

				for(u32 j = 0; j < 6; j++) {

					if(curEncoding == encodingOrder[j]) {

						if(j < bestUnicodeEncoding) {

							bestIndex = i;
							bestUnicodeEncoding = j;

						}

						break;

					}

				}

			}

			if(bestUnicodeEncoding >= 6) {
				throw LoaderException("Illegal state for subtable chooser");
			}

			bestHeader = unicodeHeaders[bestIndex];

		} else if (microsoftHeaders.size()) {

			u16 bestMicrosoftEncoding = 7;
			u32 bestIndex = -1;

			constexpr static MicrosoftEncoding encodingOrder[] = {
				MicrosoftEncoding::UnicodeFull, MicrosoftEncoding::UnicodeBMP, MicrosoftEncoding::ShiftJIS, MicrosoftEncoding::PRC, MicrosoftEncoding::Big5, MicrosoftEncoding::Wansung, MicrosoftEncoding::Johab
			};

			for(u32 i = 0; i < microsoftHeaders.size(); i++) {

				MicrosoftEncoding curEncoding = static_cast<MicrosoftEncoding>(microsoftHeaders[i].platformSpecificID);

				for(u32 j = 0; j < 7; j++) {

					if(curEncoding == encodingOrder[j]) {

						if(j < bestMicrosoftEncoding) {

							bestIndex = i;
							bestMicrosoftEncoding = j;

						}

						break;

					}

				}

			}

			if(bestMicrosoftEncoding >= 7) {
				throw LoaderException("Illegal state for subtable chooser");
			}

			bestHeader = microsoftHeaders[bestIndex];

		} else if (macintoshHeaders.size()) {

			bestHeader = macintoshHeaders[0];

		} else {

			throw LoaderException("No valid cmap subtable has been found");

		}

		//Subtable found, now parse it
		u32 targetOffset = bestHeader.offset;

		if(tableSize < targetOffset + 2) {
			throw LoaderException("Failed to load character map table: Stream size too small");
		}

		reader.getStream().seekTo(startStreamPos + targetOffset);
		u16 subtableFormat = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
		Log::info("TrueType Loader", "[Character Map Subtable] Subtable version %d", subtableFormat);
#endif

		switch(subtableFormat) {

			case 4:
				return parseSubtableVersion4(reader, tableSize - targetOffset - 2);

			case 12:
				return parseSubtableVersion12(reader, tableSize - targetOffset - 2);

			default:
				throw LoaderException("Not yet implemented");
				break;

		}

	}

}