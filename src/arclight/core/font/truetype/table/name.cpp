/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 name.cpp
 */

#include "font/truetype/truetype.hpp"
#include "stream/binaryreader.hpp"
#include "math/math.hpp"
#include "util/bool.hpp"


namespace TrueType {

    void parseNameTable(BinaryReader& reader, u32 tableSize) {

        SizeT streamSize = reader.getStream().getSize();
        SizeT offset = reader.getStream().getPosition();

        if(tableSize < 6) {
            throw LoaderException("Failed to load naming table: Stream size too small");
        }

        //TTF type 0
        u16 format = reader.read<u16>();
        u16 count = reader.read<u16>();
        u16 stringOffset = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[Name] Format: %d, Count: %d, StringOffset: 0x%X", format, count, stringOffset);
#endif

        if(Bool::none(format, 0, 1)) {
            throw LoaderException("Failed to load name table: Invalid version %d", format);
        }

        if(tableSize < static_cast<u32>(count * 12 + 6)) {
            throw LoaderException("Failed to load name table: Stream size too small");
        }

        std::vector<NameRecord> nameRecords;

        for(u16 i = 0; i < count; i++) {

            NameRecord record;
            record.platformID = reader.read<u16>();
            record.platformSpecificID = reader.read<u16>();
            record.languageID = reader.read<u16>();
            record.nameID = reader.read<u16>();
            record.length = reader.read<u16>();
            record.offset = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
            Log::info("TrueType Loader", "[Name Name] PlatformID: 0x%X, SpecificID: 0x%X, Language: %d, Name: %d, Length: 0x%X, Offset: 0x%X", 
            record.platformID, record.platformSpecificID, record.languageID, record.nameID, record.length, record.offset);
#endif

            if(streamSize < stringOffset + record.offset + record.length) {
                throw LoaderException("Failed to load name table: Stream size too small");
            }

            if(!verifyPlatformID(record.platformID, record.platformSpecificID, false)) {
                throw LoaderException("Illegal platform ID/specific ID combination (PID: %d, SID: %d)", record.platformID, record.platformSpecificID);
            }

            nameRecords.push_back(record);

        }

        //OTF type 1
        if(format == 1) {

            if(tableSize < static_cast<u32>(count * 12 + 8)) {
                throw LoaderException("Failed to load name table: Stream size too small");
            }
            
            u16 langTagCount = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
            Log::info("TrueType Loader", "[Name OTF] LangTagCount: %d", langTagCount);
#endif

            if(tableSize < static_cast<u32>(langTagCount * 4 + count * 12 + 8)) {
                throw LoaderException("Failed to load name table: Stream size too small");
            }

            for(u16 i = 0; i < langTagCount; i++) {

                u16 length = reader.read<u16>();
                u16 langTagOffset = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
                Log::info("TrueType Loader", "[Name Language Tag] Length: 0x%X, Offset: 0x%X", length, langTagOffset);
#endif

                if(streamSize < stringOffset + langTagOffset + length) {
                    throw LoaderException("Failed to load name table: Stream size too small");
                }

            }

        }

        std::string text;

        for(NameRecord& record : nameRecords) {

            //TODO: Manage encodings dependent on platform IDs, verify values
            reader.getStream().seek(offset + stringOffset + record.offset);

            text.resize(Math::max(record.length, text.size()));
            reader.read(std::span{text}, record.length);

            record.content = decodeText(static_cast<PlatformID>(record.platformID), record.platformSpecificID, text);

#ifdef ARC_FONT_DEBUG
            Log::info("TrueType Loader", "[Name String] %d: %s", record.nameID, record.content.c_str());
#endif

        }

    }

}
