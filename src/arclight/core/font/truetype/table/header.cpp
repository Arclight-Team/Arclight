/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 header.cpp
 */

#include "font/truetype/truetype.hpp"
#include "stream/binaryreader.hpp"
#include "util/bool.hpp"


namespace TrueType {

    FontHeader parseHeaderTable(BinaryReader& reader, u32 tableSize) {

        if(tableSize < 54) {
            throw LoaderException("Failed to load header table: Stream size too small");
        }

        u32 version = reader.read<u32>();
        u32 fontRevision = reader.read<u32>();
        u32 checkSumAdjustment = reader.read<u32>();
        u32 magicNumber = reader.read<u32>();
        u16 flags = reader.read<u16>();
        u16 unitsPerEm = reader.read<u16>();
        u64 created = ttfDateToUnixTime(reader.read<i64>());
        u64 modified = ttfDateToUnixTime(reader.read<i64>());
        i16 xMin = reader.read<i16>();
        i16 yMin = reader.read<i16>();
        i16 xMax = reader.read<i16>();
        i16 yMax = reader.read<i16>();
        u16 macStyle = reader.read<u16>();
        u16 lowestRecPPEM = reader.read<u16>();
        i16 fontDirectionHint = reader.read<i16>();
        i16 indexToLocFormat = reader.read<i16>();
        i16 glyphDataFormat = reader.read<i16>();

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[Header] Version 0x%08X, Revision 0x%08X", version, fontRevision);
        Log::info("TrueType Loader", "[Header] ChecksumAdj: 0x%08X, Magic: 0x%08X, Flags: 0x%04X, UnitsPerEM: %d, Created: %llu, Modified: %llu",
                    checkSumAdjustment, magicNumber, flags, unitsPerEm, created, modified);
        Log::info("TrueType Loader", "[Header] XMin: %d, YMin: %d, XMax: %d, YMax: %d, Style: 0x%04X, LowestRecPPEM: %d, Direction: %d, IndexToLocFormat: %d, GlyphDataFormat: %d",
                    xMin, yMin, xMax, yMax, macStyle, lowestRecPPEM, fontDirectionHint, indexToLocFormat, glyphDataFormat);
#endif

        if(version != 0x00010000) {
            throw LoaderException("Cannot read header table with versions newer than 1.0");
        }

        if(magicNumber != 0x5F0F3CF5) {
            throw LoaderException("Bad header table");
        }

        if(unitsPerEm < 64 || unitsPerEm > 16384) {
            throw LoaderException("Units per em out of bounds (%d)", unitsPerEm);
        }

        if(fontDirectionHint < -2 || fontDirectionHint > 2) {
            throw LoaderException("Illegal font direction hint");
        }

        if(Bool::none(indexToLocFormat, 0, 1)) {
            throw LoaderException("Index to loc format cannot be %d", indexToLocFormat);
        }

        if(glyphDataFormat != 0) {
            throw LoaderException("Glyph data format cannot be %d", glyphDataFormat);
        }

        FontHeader header;
        header.revision = fontRevision;
        header.flags = flags;
        header.unitsPerEm = unitsPerEm;
        header.created = created;
        header.modified = modified;
        header.xMin = xMin;
        header.yMin = yMin;
        header.xMax = xMax;
        header.yMax = yMax;
        header.macStyle = macStyle;
        header.lowestRecPPEM = lowestRecPPEM;
        header.fontDirectionHint = fontDirectionHint;
        header.longLocationFormat = indexToLocFormat;

        return header;

    }

}