#include "font/truetype/truetype.h"
#include "stream/binaryreader.h"




namespace TrueType {

    void parseHorizontalMetricsTable(BinaryReader& reader, u32 tableSize, std::vector<Glyph>& glyphs, u32 metricsCount) {

        u32 glyphCount = glyphs.size();

        if(metricsCount > glyphCount) {
            throw LoaderException("Horizontal metrics count cannot exceed number of glyphs (Metrics: %d, Glyphs: %d)", metricsCount, glyphCount);
        }

        if(metricsCount == 0) {
            throw LoaderException("At least one horizontal metric is required");
        }

        u32 nonmetricsCount = glyphCount - metricsCount;
        u32 requiredTableSize = 4 * metricsCount + 2 * nonmetricsCount;

        if(tableSize < requiredTableSize) {
            throw LoaderException("Failed to load horizontal metrics table: Stream size too small");
        }

        for(u32 i = 0; i < metricsCount; i++) {

            glyphs[i].advance = reader.read<u16>();
            glyphs[i].bearing = reader.read<i16>();

        }

        u16 lastAdvance = glyphs[metricsCount - 1].advance;

        for(u32 i = 0; i < nonmetricsCount; i++) {

            glyphs[metricsCount + i].advance = lastAdvance;
            glyphs[metricsCount + i].bearing = reader.read<i16>();

        }

    }

}