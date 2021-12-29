/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 maxprofile.cpp
 */

#include "font/truetype/truetype.hpp"
#include "stream/binaryreader.hpp"




namespace TrueType {

    MaximumProfile parseMaxProfileTable(BinaryReader& reader, u32 tableSize) {

        if(tableSize < 6) {
            throw LoaderException("Failed to load max profile table: Stream size too small");
        }

        u32 version = reader.read<u32>();
        u16 numGlyphs = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[Max Profile] Version: 0x%08X, GlyphCount: %d", version, numGlyphs);
#endif

        if(version == 0x00010000) {

            if(tableSize < 32) {
                throw LoaderException("Failed to load max profile table: Stream size too small");
            }

            u16 maxPoints = reader.read<u16>();
            u16 maxContours = reader.read<u16>();
            u16 maxComponentPoints = reader.read<u16>();
            u16 maxComponentContours = reader.read<u16>();
            u16 maxZones = reader.read<u16>();
            u16 maxTwilightPoints = reader.read<u16>();
            u16 maxStorage = reader.read<u16>();
            u16 maxFunctionDefs = reader.read<u16>();
            u16 maxInstructionDefs = reader.read<u16>();
            u16 maxStackElements = reader.read<u16>();
            u16 maxSizeOfInstructions = reader.read<u16>();
            u16 maxComponentElements = reader.read<u16>();
            u16 maxComponentDepth = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
            Log::info("TrueType Loader", "[Max Profile] MaxPoints: %d, MaxContours: %d, MaxComponentPoints: %d, MaxComponentContours: %d, MaxZones: %d, MaxTwilightPoints: %d", maxPoints, maxContours, maxComponentPoints, maxComponentContours, maxZones, maxTwilightPoints);
            Log::info("TrueType Loader", "[Max Profile] MaxStorage: %d, MaxFunctionDefs: %d, MaxInstructionDefs: %d, MaxStackElements: %d, MaxSizeOfInstructions: %d, MaxComponentElements: %d, MaxComponentDepth: %d", maxStorage, maxFunctionDefs, maxInstructionDefs, maxStackElements, maxSizeOfInstructions, maxComponentElements, maxComponentDepth);
#endif

        } else if(version != 0x00005000) {
            throw LoaderException("Bad max profile version");
        }

        MaximumProfile profile;
        profile.glyphCount = numGlyphs;

        return profile;

    }

}