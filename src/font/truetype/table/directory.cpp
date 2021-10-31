#include "font/truetype/truetype.h"
#include "stream/binaryreader.h"
#include "util/bool.h"

#include <array>
#include <algorithm>
#include <unordered_map>



namespace TrueType {

    constexpr inline u32 offsetSubtableSize = 12;
    constexpr inline u32 tableDirectorySize = 16;

    constexpr inline u32 scalerTypeTrueOSX = 0x74727565; //true
    constexpr inline u32 scalerTypeTrueTC  = 0x74746366; //ttfc
    constexpr inline u32 scalerTypeTrueWin = 0x00010000;


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

        if(Bool::none(scalerType, scalerTypeTrueOSX, scalerTypeTrueWin, scalerTypeTrueTC)) {
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