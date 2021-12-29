/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 directory.cpp
 */

#include "font/truetype/truetype.hpp"
#include "stream/binaryreader.hpp"
#include "util/bool.hpp"

#include <array>
#include <algorithm>
#include <unordered_map>



namespace TrueType {

    constexpr inline u32 ttcHeaderMinSize = 12;
    constexpr inline u32 ttcSignature  = 0x74746366; //ttfc

    constexpr inline u32 offsetSubtableSize = 12;
    constexpr inline u32 tableDirectorySize = 16;

    constexpr inline u32 scalerTypeTrueOSX = 0x74727565; //true
    constexpr inline u32 scalerTypeTrueWin = 0x00010000;


    std::vector<TableMap> parseSharedFileHeader(BinaryReader& reader) {

        SizeT streamSize = reader.getStream().getSize();

        if(streamSize < ttcHeaderMinSize) {
            throw LoaderException("Failed to load TrueType collection: Stream size too small");
        }

        u32 ttcTag = reader.read<u32>();
        u16 majorVersion = reader.read<u16>();
        u16 minorVersion = reader.read<u16>();
        u32 fontCount = reader.read<u32>();

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[TTC Header] Tag: 0x%X, Major: %d, Minor: %d, FontCount: %d", ttcTag, majorVersion, minorVersion, fontCount);
#endif

        if(ttcTag != ttcSignature) {
            throw LoaderException("Failed to load TrueType collection: File is not a TrueType Collection");
        }

        if(Bool::none(majorVersion, 1, 2) || minorVersion != 0) {
            throw LoaderException("Failed to load TrueType collection: Bad TTC header version");
        }

        u32 requiredSize = ttcHeaderMinSize + fontCount * 4 + (majorVersion == 2 ? 12 : 0);

        if(streamSize < requiredSize) {
            throw LoaderException("Failed to load TrueType collection: Stream size too small");
        }

        std::vector<u32> offsets;

        for(u32 i = 0; i < fontCount; i++) {
            offsets.push_back(reader.read<u32>());
        }

        if(majorVersion == 2) {

            u32 dsigTag = reader.read<u32>();
            u32 dsigLength = reader.read<u32>();
            u32 dsigOffset = reader.read<u32>();

#ifdef ARC_FONT_DEBUG
            Log::info("TrueType Loader", "[TTC Header] DSIGTag: 0x%X, DSIGLength: 0x%X, DSIGOffset: 0x%X", dsigTag, dsigLength, dsigOffset);
#endif

        }

        std::vector<TableMap> tableMaps;

        for(u32 i = 0; i < fontCount; i++) {

            reader.getStream().seek(offsets[i]);
            tableMaps.emplace_back(parseFileHeader(reader));

        }

        return tableMaps;

    }


    TableMap parseFileHeader(BinaryReader& reader) {

        SizeT streamSize = reader.getStream().getSize();

        if(streamSize < offsetSubtableSize) {
            throw LoaderException("Failed to load TrueType font: Stream size too small");
        }

        //Offset subtable
        u32 scalerType = reader.read<u32>();
        u16 numTables = reader.read<u16>();
        u16 searchRange = reader.read<u16>();
        u16 entrySelector = reader.read<u16>();
        u16 rangeShift = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[Offset Subtable] Scaler: 0x%X, Tables: %d, Range: %d, Entry: %d, Unspotted: %d", scalerType, numTables, searchRange, entrySelector, rangeShift);
#endif

        if(Bool::none(scalerType, scalerTypeTrueOSX, scalerTypeTrueWin)) {
            throw LoaderException("Failed to load TrueType font: Bad requested scaler type");
        }

        u32 totalTableDirectorySize = numTables * tableDirectorySize;

        if(streamSize < offsetSubtableSize + totalTableDirectorySize) {
            throw LoaderException("Failed to load TrueType font: Stream size too small");
        }

        //Table directory
        TableMap tableEntries;

        for(u16 i = 0; i < numTables; i++) {

            TableDirectoryEntry entry;
            entry.tag = Bits::swap32(reader.read<u32>());
            entry.checksum = reader.read<u32>();
            entry.offset = reader.read<u32>();
            entry.length = reader.read<u32>();

            if(tableEntries.contains(entry.tag)) {
                throw LoaderException("Duplicate table entry %.4s", entry.tag);
            }

            tableEntries[entry.tag] = entry;

        }

#ifdef ARC_FONT_DEBUG
        for(const auto& [tag, entry] : tableEntries) {
            Log::info("TrueType Loader", "[Entry] %.4s - Checksum: 0x%08X, Offset: 0x%X, Length: 0x%X", reinterpret_cast<const u8*>(&tag), entry.checksum, entry.offset, entry.length);
        }
#endif

        constexpr static std::array requiredTableTags = {
            TableType::CharMap, TableType::GlyphData, TableType::Header, TableType::HorizontalHeader, TableType::HorizontalMetrics, TableType::Location, TableType::MaxProfile, TableType::Name, TableType::PostScript
        };

        if(auto it = std::find_if_not(requiredTableTags.begin(), requiredTableTags.end(), [&tableEntries](u32 tag) {
            return tableEntries.contains(tag);
        }); it != requiredTableTags.end()) {
            throw LoaderException("Incomplete TTF file: Required table %.4s missing", reinterpret_cast<const u8*>(&*it));
        }

        for(const auto& [tag, entry] : tableEntries) {
            
            if(reader.getStream().getSize() < entry.offset + entry.length) {
                throw LoaderException("Failed to load TrueType font: Stream size too small");
            }

        }

        return tableEntries;

    }

}