#include "font/truetype/truetype.h"
#include "stream/binaryreader.h"



namespace TrueType {



    void parseCharacterMapTable(BinaryReader& reader, u32 tableSize) {

        if(tableSize < 4) {
            throw LoaderException("Failed to load character map table: Stream size too small");
        }

        u16 version = reader.read<u16>();
        u16 numberSubtables = reader.read<u16>();

#ifdef ARC_FONT_DEBUG
        Log::info("TrueType Loader", "[Character Map] Version: 0x%04X, NumSubtables: %d", version, numberSubtables);
#endif

        if(version != 0) {
            throw LoaderException("Failed to load character map table: Illegal character map table version"); 
        }

        if(numberSubtables == 0) {
            throw LoaderException("Failed to load character map table: Subtable count cannot be 0"); 
        }

        if(tableSize < static_cast<u32>(numberSubtables) * 8 + 4) {
            throw LoaderException("Failed to load character map table: Stream size too small");
        }

        std::vector<CMapSubtableHeader> unicodeHeaders;
        std::vector<CMapSubtableHeader> uvsHeaders;
        std::vector<CMapSubtableHeader> microsoftHeaders;
        std::vector<CMapSubtableHeader> symbolHeaders;
        std::vector<CMapSubtableHeader> macintoshHeaders;

        for(u32 i = 0; i < numberSubtables; i++) {

            CMapSubtableHeader header;
            header.platformID = reader.read<u16>();
            header.platformSpecificID = reader.read<u16>();
            header.offset = reader.read<u32>();

#ifdef ARC_FONT_DEBUG
            Log::info("TrueType Loader", "[Character Map Subtable] PlatformID: %d, SpecificID: %d, Offset: 0x%08X", header.platformID, header.platformSpecificID, header.offset);
#endif

            if(!verifyPlatformID(header.platformID, header.platformSpecificID, true)) {
                throw LoaderException("Illegal platform ID/specific ID combination (PID: %d, SID: %d)", header.platformID, header.platformSpecificID);
            }

            PlatformID id = static_cast<PlatformID>(header.platformID);

            switch(id) {

                case PlatformID::Unicode:

                    if(header.platformSpecificID == 14) {
                        uvsHeaders.push_back(header);
                    } else {
                        unicodeHeaders.push_back(header);
                    }

                    break;

                case PlatformID::Macintosh:
                    macintoshHeaders.push_back(header);
                    break;

                case PlatformID::Microsoft:

                    if(static_cast<MicrosoftEncoding>(header.platformSpecificID) == MicrosoftEncoding::Symbol) {
                        symbolHeaders.push_back(header);
                    } else {
                        microsoftHeaders.push_back(header);
                    }

                    break;

                default:
                    arc_force_assert("Illegal platform ID");
                    macintoshHeaders.push_back(header);
                    break;

            }

        }

        //Choose the best subtable for our use
        CMapSubtableHeader bestHeader;
        bool useUvs = false;

        if(symbolHeaders.size()) {

            bestHeader = symbolHeaders[0];

        } else if (unicodeHeaders.size()) {

            if(uvsHeaders.size()) {
                useUvs = true;
            }

            u16 bestUnicodeEncoding = -1;
            u32 bestIndex = -1;

            constexpr static UnicodeEncoding encodingOrder[] = {
                UnicodeEncoding::Unicode2Full, UnicodeEncoding::Unicode2BMP, UnicodeEncoding::Version11, UnicodeEncoding::Version10, UnicodeEncoding::ISO10646, UnicodeEncoding::LastResort
            };

            for(u32 i = 1; i < unicodeHeaders.size(); i++) {

                UnicodeEncoding curEncoding = static_cast<UnicodeEncoding>(unicodeHeaders[i].platformSpecificID);

                for(u32 j = 0; j < 6; j++) {

                    if(curEncoding == encodingOrder[j]) {

                        if(j < bestUnicodeEncoding) {

                            bestIndex = i;
                            bestUnicodeEncoding = j;

                        }

                        break;

                    }

                }

            }

            if(bestUnicodeEncoding == -1) {
                throw LoaderException("Illegal state for subtable chooser");
            }

            bestHeader = unicodeHeaders[bestIndex];

        } else if (microsoftHeaders.size()) {

            u16 bestMicrosoftEncoding = -1;
            u32 bestIndex = -1;

            constexpr static MicrosoftEncoding encodingOrder[] = {
                MicrosoftEncoding::UnicodeFull, MicrosoftEncoding::UnicodeBMP, MicrosoftEncoding::ShiftJIS, MicrosoftEncoding::PRC, MicrosoftEncoding::Big5, MicrosoftEncoding::Wansung, MicrosoftEncoding::Johab
            };

            for(u32 i = 1; i < unicodeHeaders.size(); i++) {

                MicrosoftEncoding curEncoding = static_cast<MicrosoftEncoding>(microsoftHeaders[i].platformSpecificID);

                for(u32 j = 0; j < 7; j++) {

                    if(curEncoding == encodingOrder[j]) {

                        if(j < bestMicrosoftEncoding) {

                            bestIndex = i;
                            bestMicrosoftEncoding = j;

                        }

                        break;

                    }

                }

            }

            if(bestMicrosoftEncoding == -1) {
                throw LoaderException("Illegal state for subtable chooser");
            }

            bestHeader = microsoftHeaders[bestIndex];

        } else if (macintoshHeaders.size()) {

            bestHeader = macintoshHeaders[0];

        } else {

            throw LoaderException("No valid cmap subtable has been found");

        }

    }

}