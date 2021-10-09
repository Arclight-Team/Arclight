#include "font/truetype/truetype.h"
#include "stream/binaryreader.h"



namespace TrueType {

    struct SimpleGlyphFlags {

        enum {
            OnCurve = 0x01,
            XShort = 0x02,
            YShort = 0x04,
            Repeat = 0x08,
            XExt = 0x10,
            YExt = 0x20,
            Overlap = 0x40
        };

    };


    std::vector<Glyph> parseGlyphTable(BinaryReader& reader, u32 tableSize, const std::vector<u32>& glyphOffsets) {

        std::vector<Glyph> glyphs;
        glyphs.reserve(glyphOffsets.size());

        SizeT glyfStart = reader.getStream().getPosition();

        std::vector<u16> endpoints;
        std::vector<u8> instructions;
        std::vector<u8> flags;
        std::vector<Vec2i> points;

        for(u32 offset : glyphOffsets) {

            u32 requiredSize = 10;

            if(tableSize < offset + requiredSize) {
                throw LoaderException("Failed to load glyph table: Stream size too small");
            }

            reader.getStream().seek(glyfStart + offset);

            i16 numberOfContours = reader.read<i16>();
            i16 xMin = reader.read<i16>();
            i16 yMin = reader.read<i16>();
            i16 xMax = reader.read<i16>();
            i16 yMax = reader.read<i16>();

            bool compound = numberOfContours < 0;
            u32 contours = compound ? 0 : numberOfContours;

            if(xMax < xMin || yMax < yMin) {
                throw LoaderException("Failed to load glyph table: Illegal glyph bounds");
            }

            Glyph glyph;
            glyph.compound = compound;
            glyph.contours = contours;
            glyph.xMin = xMin;
            glyph.yMin = yMin;
            glyph.xMax = xMax;
            glyph.yMax = yMax;

            if(compound) {



            } else {

                requiredSize += contours * 2 + 2;

                if(tableSize < offset + requiredSize) {
                    throw LoaderException("Failed to load glyph table: Stream size too small");
                }

                endpoints.resize(contours);
                reader.read<u16>(endpoints);

                u16 instructionLength = reader.read<u16>();
                requiredSize += instructionLength;

                if(tableSize < offset + requiredSize) {
                    throw LoaderException("Failed to load glyph table: Stream size too small");
                }

                instructions.resize(instructionLength);
                reader.read<u8>(instructions);

                if(contours) {

                    u32 pointCount = endpoints.back() + 1;
                    flags.resize(pointCount);

                    u32 i = 0;
                    u32 xLength = 0;
                    u32 yLength = 0;

                    while (i < pointCount) {

                        if(tableSize < offset + ++requiredSize) {
                            throw LoaderException("Failed to load glyph table: Stream size too small");
                        }

                        u8 flag = reader.read<u8>();
                        u32 count = 1;
                        
                        if(flag & SimpleGlyphFlags::Repeat) {

                            if(tableSize < offset + ++requiredSize) {
                                throw LoaderException("Failed to load glyph table: Stream size too small");
                            }

                            count += reader.read<u8>();

                        }

                        if(i + count > pointCount) {
                            throw LoaderException("Failed to load glyph table: Illegal repeat count");
                        }


                        xLength += flag & SimpleGlyphFlags::XShort ? count : flag & SimpleGlyphFlags::XExt ? 0 : count * 2;
                        yLength += flag & SimpleGlyphFlags::YShort ? count : flag & SimpleGlyphFlags::YExt ? 0 : count * 2;

                        std::fill_n(flags.begin() + i, count, flag);
                        i += count;

                    }

                    requiredSize += xLength + yLength;

                    if(tableSize < offset + requiredSize) {
                        throw LoaderException("Failed to load glyph table: Stream size too small");
                    }

                    points.resize(pointCount);

                    Vec2i lastPoint;

                    for(u32 j = 0; j < pointCount; j++) {

                        i32& x = points[j].x;
                        u8 flag = flags[j];

                        if(flag & SimpleGlyphFlags::XShort) {

                            u8 pointOffset = reader.read<u8>();
                            x = flag & SimpleGlyphFlags::XExt ? lastPoint.x + pointOffset : lastPoint.x - pointOffset;

                        } else {

                            x = flag & SimpleGlyphFlags::XExt ? lastPoint.x : lastPoint.x + reader.read<i16>();

                        }

                        lastPoint.x = x;

                    }

                    for(u32 j = 0; j < pointCount; j++) {

                        i32& y = points[j].y;
                        u8 flag = flags[j];

                        if(flag & SimpleGlyphFlags::YShort) {

                            u8 pointOffset = reader.read<u8>();
                            y = flag & SimpleGlyphFlags::YExt ? lastPoint.y + pointOffset : lastPoint.y - pointOffset;

                        } else {

                            y = flag & SimpleGlyphFlags::YExt ? lastPoint.y : lastPoint.y + reader.read<i16>();

                        }

                        lastPoint.y = y;

                    }

                    glyph.points = points;

                }

            }

            glyphs.push_back(glyph);

        }

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[Glyph] Loaded %d entries", glyphOffsets.size());
#endif

        return glyphs;

    }

}