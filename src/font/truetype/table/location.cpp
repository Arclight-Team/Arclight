#include "font/truetype/truetype.h"
#include "stream/binaryreader.h"



namespace TrueType {

    std::vector<u32> parseGlyphLocationTable(BinaryReader& reader, u32 tableSize, u32 glyphCount, bool longVersion) {

        if(glyphCount == 0) {
            throw LoaderException("Failed to load location table: Glyph count cannot be 0");
        }

        if(tableSize < (glyphCount + 1) * (longVersion ? 4 : 2)) {
            throw LoaderException("Failed to load header table: Stream size too small");
        }

        std::vector<u32> glyphOffsets;
        glyphOffsets.reserve(glyphCount);

        if(longVersion) {

            for(u32 i = 0; i < glyphCount; i++) {

                u32 offset = reader.read<u32>();
                glyphOffsets.push_back(offset);

            }

        } else {

            for(u32 i = 0; i < glyphCount; i++) {

                u32 offset = reader.read<u16>() * 2;
                glyphOffsets.push_back(offset);

            }

        }

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[Location] Loaded %d entries", glyphOffsets.size());
#endif

        return glyphOffsets;

    }

}