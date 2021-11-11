#include "loader.h"
#include "truetype.h"
#include "stream/binaryreader.h"
#include "util/log.h"



namespace TrueType {

    Font loadFontInternal(BinaryReader& reader, TableMap& tables) {

        InputStream& stream = reader.getStream();

        stream.seek(tables[TableType::Name].offset);
        parseNameTable(reader, tables[TableType::Name].length);

        stream.seek(tables[TableType::Header].offset);
        FontHeader fontHeader = parseHeaderTable(reader, tables[TableType::Header].length);

        stream.seek(tables[TableType::MaxProfile].offset);
        MaximumProfile maxp = parseMaxProfileTable(reader, tables[TableType::MaxProfile].length);

        stream.seek(tables[TableType::PostScript].offset);
        parsePostScriptTable(reader, tables[TableType::PostScript].length, maxp.glyphCount);

        stream.seek(tables[TableType::HorizontalHeader].offset);
        HorizontalHeader hhead = parseHorizontalHeaderTable(reader, tables[TableType::HorizontalHeader].length);

        stream.seek(tables[TableType::CharMap].offset);
        std::unordered_map<u32, u32> charMap = parseCharacterMapTable(reader, tables[TableType::CharMap].length);

        stream.seek(tables[TableType::Location].offset);
        std::vector<u32> glyphOffsets = parseGlyphLocationTable(reader, tables[TableType::Location].length, maxp.glyphCount, fontHeader.longLocationFormat);

        stream.seek(tables[TableType::GlyphData].offset);
        std::vector<Glyph> glyphs = parseGlyphTable(reader, tables[TableType::GlyphData].length, glyphOffsets);

        stream.seek(tables[TableType::HorizontalMetrics].offset);
        parseHorizontalMetricsTable(reader, tables[TableType::HorizontalMetrics].length, glyphs, hhead.metricsCount);

        return {charMap, glyphs};

    }

    Font loadFont(InputStream& stream) {

        BinaryReader reader(stream, true, ByteOrder::Big);

        try {

            TableMap tables = parseFileHeader(reader);
            return loadFontInternal(reader, tables);            

        } catch (const LoaderException& e) {
            Log::error("TrueType Loader", e.what());
            return {};
        }

    }


    std::vector<Font> loadFontCollection(InputStream& stream) {

        BinaryReader reader(stream, true, ByteOrder::Big);

        try {

            std::vector<TableMap> tableMaps = parseSharedFileHeader(reader);
            std::vector<Font> fonts;

            for(TableMap& tables : tableMaps) {
                fonts.emplace_back(loadFontInternal(reader, tables));
            }

            return fonts;

        } catch (const LoaderException& e) {
            Log::error("TrueType Loader", e.what());
            return {};
        }

    }


}