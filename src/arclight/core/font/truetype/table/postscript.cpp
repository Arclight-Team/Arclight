#include "font/truetype/truetype.hpp"
#include "stream/binaryreader.hpp"



namespace TrueType {

    void parsePostScriptTable(BinaryReader& reader, u32 tableSize, u32 glyphCount) {

        if(tableSize < 32) {
            throw LoaderException("Failed to load PostScript table: Stream size too small");
        }

        u32 format = reader.read<u32>();
        u32 italicAngle = reader.read<u32>();
        i16 underlinePosition = reader.read<i16>();
        i16 underlineThickness = reader.read<i16>();
        u32 isFixedPitch = reader.read<u32>();
        u32 minMemType42 = reader.read<u32>();
        u32 maxMemType42 = reader.read<u32>();
        u32 minMemType1 = reader.read<u32>();
        u32 maxMemType1 = reader.read<u32>();

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[PostScript] Version: 0x%08X, ItalicAngle: %d, UnderlinePosition: %d, UnderlineThickness: %d, IsFixedPitch: %d, MinMemType42: %d, MaxMemType42: %d, MinMemType1: %d, MaxMemType1: %d",
                    format, italicAngle, underlinePosition, underlineThickness, isFixedPitch, minMemType42, maxMemType42, minMemType1, maxMemType1);
#endif

        switch(format) {

            case 0x00010000:
            case 0x00020000:
            case 0x00025000:
            case 0x00030000:
            case 0x00040000:
                //As of now, the version-dependent post header is of no use so skip here
                break;

            default:
                throw LoaderException("Illegal PostScript header version of 0x%08X", format);
                break;

        }

    }

}