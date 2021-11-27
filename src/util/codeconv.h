#pragma once

#include <string>

#include "types.h"
#include "util/unicode.h"
#include "util/string.h"
#include "util/bits.h"



namespace CodeConv {

    using Unicode::Encoding;
    
    extern const u32 sjisUnicodeTable[0x3100];
    extern const u32 big5UnicodeTable[0x5FA1];
    extern const u32 wansungUnicodeTable[0x5E00];
    extern const u32 johabUnicodeTable[0x7C84];


    //UTF inter-conversion
    template<Encoding From, Encoding To>
    constexpr static std::string convertUnicode(const std::string_view& in) {

        std::string convertedString;
        convertedString.reserve(in.size());

        u32 i = 0;

        while(i < in.size()) {

            u32 parsedBytes = 0;
            u32 codepoint = toCodepoint<From>({&in[i], in.size() - i - 1}, parsedBytes);

            if(!parsedBytes) {
                //In case of error, skip byte and hope for the best
                parsedBytes++;
            }

            convertedString += Unicode::toUTF<To>(codepoint);
            i += parsedBytes;

        }

        return convertedString;

    }


    //Standard Shift-JIS to Unicode
    template<Encoding E>
    std::string fromShiftJIS(const std::string_view& in) {

        std::string unicodeString;
        unicodeString.reserve(in.size() * 2);

        u32 i = 0;

        while(i < in.size()) {

            u8 c = in[i++];
            u32 high = c >> 4;

            switch(high) {

                case 0x8:
                case 0x9:

                    if(i == in.size()) {
                        break;
                    }

                    unicodeString += Unicode::toUTF<E>(sjisUnicodeTable[0x100 + ((c & 0x1F) << 8) | in[i++]]);
                    break;

                case 0xE:

                    if(i == in.size()) {
                        break;
                    }

                    unicodeString += Unicode::toUTF<E>(sjisUnicodeTable[0x2100 + ((c & 0x1F) << 8) | in[i++]]);
                    break;

                default:
                    unicodeString += Unicode::toUTF<E>(sjisUnicodeTable[c]);
                    break;

            }

        }

        return unicodeString;

    }


    //Standard Big5 to Unicode
    template<Encoding E>
    std::string fromBig5(const std::string_view& in) {

        std::string unicodeString;
        unicodeString.reserve(in.size() * 2);

        u32 i = 0;

        while(i < in.size()) {

            u8 c = in[i++];

            if(c >= 0xA1) {

                if(i == in.size()) {
                    break;
                }

                unicodeString += Unicode::toUTF<E>(big5UnicodeTable[0xA1 + (((c - 0xA1) << 8) | in[i++])]);

            } else {

                unicodeString += Unicode::toUTF<E>(big5UnicodeTable[c]);

            }

        }

        return unicodeString;

    }


    //Wansung with UHC extensions to Unicode
    template<Encoding E>
    std::string fromWansung(const std::string_view& in) {

        std::string unicodeString;
        unicodeString.reserve(in.size() * 2);

        u32 i = 0;

        while(i < in.size()) {

            u8 c = in[i++];

            if(c >= 0x21 || c < 0x7E) {

                if(i == in.size()) {
                    break;
                }

                unicodeString += Unicode::toUTF<E>(wansungUnicodeTable[0x100 + (((c - 0x21) << 8) | in[i++])]);

            } else {

                unicodeString += Unicode::toUTF<E>(wansungUnicodeTable[c]);

            }

        }

        return unicodeString;

    }


    //Johab to Unicode
    template<Encoding E>
    std::string fromJohab(const std::string_view& in) {

        std::string unicodeString;
        unicodeString.reserve(in.size() * 2);

        u32 i = 0;

        while(i < in.size()) {

            u8 c = in[i++];

            if(c >= 0x84) {

                if(i == in.size()) {
                    break;
                }

                unicodeString += Unicode::toUTF<E>(johabUnicodeTable[0x84 + (((c - 0x84) << 8) | in[i++])]);

            } else {

                unicodeString += Unicode::toUTF<E>(johabUnicodeTable[c]);

            }

        }

        return unicodeString;

    }

}