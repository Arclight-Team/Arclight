#include "font/truetype/truetype.hpp"
#include "stream/binaryreader.hpp"
#include "math/matrix.hpp"



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

    struct ComponentGlyphFlags {

        enum {
            WordArgs = 0x1,
            XYArgs = 0x2,
            RoundXY = 0x4,
            HasScale = 0x8,
            MoreComponents = 0x20,
            DualScales = 0x40,
            MatrixTransform = 0x80,
            HasInstructions = 0x100,
            UseComponentMetrics = 0x200,
            OverlapCompound = 0x400,
            ScaledComponentOffset = 0x800,
            UnscaledComponentOffset = 0x1000
        };

    };


    std::vector<Glyph> parseGlyphTable(BinaryReader& reader, u32 tableSize, const std::vector<u32>& glyphOffsets) {

        std::vector<Glyph> glyphs;
        glyphs.reserve(glyphOffsets.size());

        SizeT glyfStart = reader.getStream().getPosition();

        std::vector<Vec2ui> contours;
        std::vector<u8> flags;
        std::vector<Vec2i> points;
        std::vector<bool> onCurve;
        std::vector<Vec4i> componentDispatches;    //x = glyph ID, y = component ID, z/w = indices

        for(u32 n = 0; n < glyphOffsets.size(); n++) {

            u32 offset = glyphOffsets[n];

            if(offset == noOutlineGlyphOffset) {
                glyphs.emplace_back();
                continue;
            }

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
            u32 contourCount = compound ? 0 : numberOfContours;

            if(xMax < xMin || yMax < yMin) {
                throw LoaderException("Failed to load glyph table: Illegal glyph bounds");
            }

            Glyph glyph;
            glyph.compound = compound;
            glyph.xMin = xMin;
            glyph.yMin = yMin;
            glyph.xMax = xMax;
            glyph.yMax = yMax;
            glyph.advance = xMax - xMin;
            glyph.bearing = 0;

            if(compound) {

                u16 flag = 0;
                u32 metricsID = 0;
                u32 componentID = 0;
                bool hasInstructions = false;

                glyph.data = std::vector<Glyph::Component>();

                do {

                    requiredSize += 4;

                    if(tableSize < offset + requiredSize) {
                        throw LoaderException("Failed to load glyph table: Stream size too small");
                    }

                    flag = reader.read<u16>();
                    u16 glyphIndex = reader.read<u16>();
                    i32 arg0, arg1;

                    bool words = flag & ComponentGlyphFlags::WordArgs;
                    bool sgnd = flag & ComponentGlyphFlags::XYArgs;

                    bool round = flag & ComponentGlyphFlags::RoundXY;   //TODO: Apply correct rounding
                    //TODO: Apply correct base scaling

                    hasInstructions |= static_cast<bool>(flag & ComponentGlyphFlags::HasInstructions);

                    if(flag & ComponentGlyphFlags::UseComponentMetrics) {
                        metricsID = componentID;
                    }
                    
                    enum class ScaleData {
                        None,
                        Single,
                        Dual,
                        Matrix
                    };

                    ScaleData scaleData = flag & ComponentGlyphFlags::HasScale ? ScaleData::Single : (flag & ComponentGlyphFlags::DualScales ? ScaleData::Dual : (flag & ComponentGlyphFlags::MatrixTransform ? ScaleData::Matrix : ScaleData::None));
                    u32 scalesToFetch = 0;

                    switch(scaleData) {

                        default:
                        case ScaleData::None:
                            scalesToFetch = 0;
                            break;

                        case ScaleData::Single:
                            scalesToFetch = 1;
                            break;

                        case ScaleData::Dual:
                            scalesToFetch = 2;
                            break;

                        case ScaleData::Matrix:
                            scalesToFetch = 4;
                            break;

                    }

                    requiredSize += scalesToFetch * 2 + (words + 1) * 2;

                    if(tableSize < offset + requiredSize) {
                        throw LoaderException("Failed to load glyph table: Stream size too small");
                    }

                    if(words) {

                        if(sgnd) {
                            
                            arg0 = reader.read<i16>();
                            arg1 = reader.read<i16>();

                        } else {

                            arg0 = reader.read<u16>();
                            arg1 = reader.read<u16>();

                        }

                    } else {

                        if(sgnd) {
                            
                            arg0 = reader.read<i8>();
                            arg1 = reader.read<i8>();

                        } else {

                            arg0 = reader.read<u8>();
                            arg1 = reader.read<u8>();

                        }

                    }

                    Mat2d transform;

                    switch(scaleData) {

                        default:
                        case ScaleData::None:
                            break;

                        case ScaleData::Single:
                            {
                                double scale = reader.read<i16>() / 16384.0;

                                transform[0][0] = scale;
                                transform[1][1] = scale;
                            }
                            break;

                        case ScaleData::Dual:
                            {
                                double scaleX = reader.read<i16>() / 16384.0;
                                double scaleY = reader.read<i16>() / 16384.0;

                                transform[0][0] = scaleX;
                                transform[1][1] = scaleY;
                            }
                            break;

                        case ScaleData::Matrix:

                            transform = {
                                reader.read<i16>() / 16384.0, reader.read<i16>() / 16384.0,
                                reader.read<i16>() / 16384.0, reader.read<i16>() / 16384.0
                            };

                            break;

                    }

                    Glyph::Component component;
                    component.transform = transform;
                    component.glyphIndex = glyphIndex;

                    if(sgnd) {
                        component.offset = Vec2d(arg0, arg1);
                    } else {
                        componentDispatches.emplace_back(n, componentID, arg0, arg1);
                    }

                    glyph.getGlyphComponents().push_back(component);

                    componentID++;

                } while(flag & ComponentGlyphFlags::MoreComponents);

                if(hasInstructions) {

                    u16 instructionLength = reader.read<u16>();
                    requiredSize += instructionLength;

                    if(tableSize < offset + requiredSize) {
                        throw LoaderException("Failed to load glyph table: Stream size too small");
                    }

                    glyph.instructions.resize(instructionLength);
                    reader.read<u8>(glyph.instructions);

                }

            } else {

                glyph.data = Glyph::Data();

                requiredSize += contourCount * 2 + 2;

                if(tableSize < offset + requiredSize) {
                    throw LoaderException("Failed to load glyph table: Stream size too small");
                }

                contours.resize(contourCount);

                u32 lastContourStart = 0;

                for(u32 i = 0; i < contourCount; i++) {

                    u32 contourEnd = reader.read<u16>();
                    contours[i] = Vec2ui(lastContourStart, contourEnd);
                    lastContourStart = contourEnd + 1;

                }

                u16 instructionLength = reader.read<u16>();
                requiredSize += instructionLength;

                if(tableSize < offset + requiredSize) {
                    throw LoaderException("Failed to load glyph table: Stream size too small");
                }

                glyph.instructions.resize(instructionLength);
                reader.read<u8>(glyph.instructions);

                if(contourCount) {

                    u32 pointCount = contours.back().y + 1;
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
                    onCurve.resize(pointCount);

                    Vec2i lastPoint;

                    for(u32 j = 0; j < pointCount; j++) {

                        i32& x = points[j].x;
                        u8 flag = flags[j];
                        onCurve[j] = flag & SimpleGlyphFlags::OnCurve;

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

                    auto& glyphData = glyph.getGlyphData();
                    glyphData.points = points;
                    glyphData.contours = contours;
                    glyphData.onCurve = onCurve;

                }

            }

            glyphs.push_back(glyph);

        }

        for(const Vec4i& dc : componentDispatches) {

            Glyph::Component& c = glyphs[dc.x].getGlyphComponents()[dc.y];
            u32 referenceGlyphID = c.glyphIndex;

            if(referenceGlyphID >= glyphs.size()) {
                throw LoaderException("Failed to load glyph table: Component glyph reference index out of bounds");
            }

            const Glyph& refGlyph = glyphs[referenceGlyphID];

            if(refGlyph.compound) {
                throw LoaderException("Failed to load glyph table: Component reference glyph cannot be a compound glyph");
            }

            const Glyph::Data& data = refGlyph.getGlyphData();

            if(data.points.size() == 0) {
                throw LoaderException("Failed to load glyph table: Component reference point array is empty");
            }

            i32 x = dc.z;
            i32 y = dc.w;

            if(!Math::inRange(x, 0, data.points.size() - 1) || !Math::inRange(y, 0, data.points.size() - 1)) {
                throw LoaderException("Failed to load glyph table: Component reference points are out of bounds");
            }

            c.offset = data.points[y] - data.points[x];

        }

        for(Glyph& glyph : glyphs) {

            if(!glyph.compound) {
                continue;
            }

            std::vector<Glyph::Component>& components = glyph.getGlyphComponents();

            for(Glyph::Component& component : components) {

                constexpr static double transformThreshold = 33.0 / 65536.0;

                const Mat2d& transform = component.transform;
                double a = Math::abs(transform[0][0]);
                double b = Math::abs(transform[0][1]);
                double c = Math::abs(transform[1][0]);
                double d = Math::abs(transform[1][1]);

                double m0 = Math::max(a, b);
                double n0 = Math::max(c, d);
                double m = Math::lessEqual(Math::abs(a - c), transformThreshold) ? 2 * m0 : m0;
                double n = Math::lessEqual(Math::abs(b - d), transformThreshold) ? 2 * n0 : n0;

                component.offset *= Vec2d(m, n);

            }

        }

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[Glyph] Loaded %d entries", glyphOffsets.size());
#endif

        return glyphs;

    }

}