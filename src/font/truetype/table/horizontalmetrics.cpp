#include "font/truetype/truetype.h"
#include "stream/binaryreader.h"




namespace TrueType {

    std::vector<HorizontalMetric> parseHorizontalMetricsTable(BinaryReader& reader, u32 tableSize, const HorizontalHeader& header, u32 glyphCount) {

        u32 metricsCount = header.metricsCount;

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

        std::vector<HorizontalMetric> metrics;
        metrics.reserve(glyphCount);

        for(u32 i = 0; i < metricsCount; i++) {

            HorizontalMetric metric;
            metric.advance = reader.read<u16>();
            metric.bearing = reader.read<i16>();

            metrics.push_back(metric);

        }

        u16 lastAdvance = metrics.back().advance;

        for(u32 i = 0; i < nonmetricsCount; i++) {

            HorizontalMetric metric;
            metric.advance = lastAdvance;
            metric.bearing = reader.read<i16>();

            metrics.push_back(metric);

        }

        return metrics;

    }

}