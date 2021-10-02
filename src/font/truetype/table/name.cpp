#include "font/truetype/truetype.h"
#include "stream/binaryreader.h"
#include "math/math.h"
#include "util/bool.h"


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

        std::vector<char> textBuffer;

        for(NameRecord& record : nameRecords) {

            //TODO: Manage encodings dependent on platform IDs, verify values
            reader.getStream().seek(offset + stringOffset + record.offset);

            textBuffer.resize(Math::max(record.length, textBuffer.size()));
            reader.read(std::span<char>{textBuffer}, record.length);
            record.content = std::string(textBuffer.data(), record.length);

#ifdef ARC_FONT_DEBUG
            Log::info("TrueType Loader", "[Name String] %d: %s", record.nameID, record.content.c_str());
#endif

        }

    }

}
