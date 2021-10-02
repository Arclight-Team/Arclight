#include "loader.h"
#include "truetype.h"
#include "stream/binaryreader.h"
#include "util/log.h"



namespace TrueType {

    void loadFont(InputStream& stream) {

        BinaryReader reader(stream, true, ByteOrder::Big);
        SizeT streamSize = stream.getSize();

        try {

            TableMap tables = parseFileHeader(reader);

            stream.seek(tables[TableType::Name].offset);
            parseNameTable(reader, tables[TableType::Name].length);

            stream.seek(tables[TableType::Header].offset);
            parseHeaderTable(reader, tables[TableType::Header].length);

            stream.seek(tables[TableType::MaxProfile].offset);
            MaximumProfile maxp = parseMaxProfileTable(reader, tables[TableType::MaxProfile].length);

            stream.seek(tables[TableType::HorizontalHeader].offset);
            HorizontalHeader hhead = parseHorizontalHeaderTable(reader, tables[TableType::HorizontalHeader].length);

            stream.seek(tables[TableType::HorizontalMetrics].offset);
            std::vector<HorizontalMetric> metrics = parseHorizontalMetricsTable(reader, tables[TableType::HorizontalMetrics].length, hhead, maxp.glyphCount);

            for(const HorizontalMetric& metric : metrics) {
                Log::info("TrueType Debug", "Advance: %d, Bearing: %d", metric.advance, metric.bearing);
            }

        } catch (LoaderException& e) {
            Log::error("TrueType Loader", e.what());
            return;
        }

    }

}