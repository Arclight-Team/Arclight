#pragma once

#include <string>
#include "types.h"
#include "util/string.h"
#include "util/bits.h"



namespace CodeConv {

    enum class Encoding {
        UTF8,
        UTF16LE,
        UTF16BE,
        UTF32LE,
        UTF32BE
    };
    
    extern const u32 sjisUnicodeTable[0x3100];
    extern const u32 big5UnicodeTable[0x5FA1];
    extern const u32 wansungUnicodeTable[0x5E00];
    extern const u32 johabUnicodeTable[0x7C84];


    //Codepoint to UTF
    template<Encoding E>
    constexpr static std::string toUTF(u32 codepoint) {

        if constexpr (E == Encoding::UTF8) {

            if(codepoint < 0x80) {

                return String::fromChars(codepoint);

            } else if (codepoint < 0x800) {

                return String::fromChars((codepoint >> 6) | 0xC0, (codepoint & 0x3F) | 0x80);

            } else if (codepoint < 0x10000) {

                return String::fromChars((codepoint >> 12) | 0xE0, (codepoint >> 6 & 0x3F) | 0x80, (codepoint & 0x3F) | 0x80);

            } else {

                return String::fromChars((codepoint >> 18) | 0xF0, (codepoint >> 12 & 0x3F) | 0x80, (codepoint >> 6 & 0x3F) | 0x80, (codepoint & 0x3F) | 0x80);

            }

        } else if constexpr (E == Encoding::UTF16LE || E == Encoding::UTF16BE) {

            constexpr static bool convert = Bits::requiresEndianConversion(E == Encoding::UTF16LE ? ByteOrder::Little : ByteOrder::Big);

            if (codepoint >= 0x10000) {

                u16 lowSurrogate = 0xDC00 + codepoint & 0x3FF;
                u16 highSurrogate = 0xD800 + codepoint >> 10;

                if constexpr (convert) {
                    lowSurrogate = Bits::swap16(lowSurrogate);
                    highSurrogate = Bits::swap16(highSurrogate);
                }

                return String::fromChars(highSurrogate & 0xFF, highSurrogate >> 8 & 0xFF, lowSurrogate & 0xFF, lowSurrogate >> 8 & 0xFF);

            } else {

                u16 c = codepoint;

                if constexpr (convert) {
                    c = Bits::swap16(c);
                }

                return String::fromChars(c & 0xFF, c >> 8 & 0xFF);

            }

        } else if constexpr (E == Encoding::UTF32LE || E == Encoding::UTF32BE) {

            constexpr static bool convert = Bits::requiresEndianConversion(E == Encoding::UTF32LE ? ByteOrder::Little : ByteOrder::Big);

            if constexpr (convert) {
                codepoint = Bits::swap32(codepoint);
            }

            return String::fromChars(codepoint & 0xFF, codepoint >> 8 & 0xFF, codepoint >> 16 & 0xFF, codepoint >> 24 & 0xFF);

        }

    }


    //UTF to codepoint
    template<Encoding E>
    constexpr static u32 toCodepoint(const std::string_view& in) {
        u32 dummy;
        return toCodepoint<E>(in, dummy);
    }

    template<Encoding E>
    constexpr static u32 toCodepoint(const std::string_view& in, u32& readBytes) {

        constexpr static u32 errorCodepoint = 0xFFFD;

        if constexpr (E == Encoding::UTF8) {

            if(in.size() < 1) { readBytes = 0; return errorCodepoint; }

            u8 c0 = in[0];

            if(c0 < 0x80) {

                readBytes = 1;
                return c0;

            } else if (c0 < 0xE0) {

                if(in.size() < 2) { readBytes = 0; return errorCodepoint; }

                readBytes = 2;
                return (c0 & 0x1F) << 6 | in[1] & 0x3F;

            } else if (c0 < 0xF0) {
                
                if(in.size() < 3) { readBytes = 0; return errorCodepoint; }

                readBytes = 3;
                return (c0 & 0x0F) << 12 | (in[1] & 0x3F) << 6 | in[2] & 0x3F;

            } else {

                if(in.size() < 4) { readBytes = 0; return errorCodepoint; }

                //Assume we have a valid UTF-8 high byte
                readBytes = 4;
                return (c0 & 0x07) << 18 | (in[1] & 0x3F) << 12 | (in[2] & 0x3F) << 6 | in[3] & 0x3F;

            }

        } else if constexpr (E == Encoding::UTF16LE || E == Encoding::UTF16BE) {

            constexpr static bool convert = Bits::requiresEndianConversion(E == Encoding::UTF16LE ? ByteOrder::Little : ByteOrder::Big);

            if(in.size() < 2) { readBytes = 0; return errorCodepoint; }

            u16 c0 = in[1] << 8 | in[0];

            if constexpr (convert) {
                c0 = Bits::swap16(c0);
            }

            if (c0 >= 0xD800 && c0 < 0xDC00) {

                if(in.size() < 4) { readBytes = 0; return errorCodepoint; }

                u16 c1 = in[3] << 8 | in[2];

                if constexpr (convert) {
                    c1 = Bits::swap16(c1);
                }

                //Assume we have a low-surrogate in c1
                readBytes = 4;
                return (c0 & 0x3FF) << 10 | (c1 & 0x3FF);

            } else {

                readBytes = 2;
                return c0;

            }

        } else if constexpr (E == Encoding::UTF32LE || E == Encoding::UTF32BE) {

            constexpr static bool convert = Bits::requiresEndianConversion(E == Encoding::UTF32LE ? ByteOrder::Little : ByteOrder::Big);

            if(in.size() < 4) { readBytes = 0; return errorCodepoint; }

            u32 codepoint = in[3] << 24 | in[2] << 16 | in[1] << 8 | in[0];

            if constexpr (convert) {
                codepoint = Bits::swap32(codepoint);
            }

            readBytes = 4;
            return codepoint;

        }

    }


    //UTF inter-conversion
    template<Encoding From, Encoding To>
    constexpr static u32 convertUnicode(const std::string_view& in) {

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

            convertedString += toUTF<To>(codepoint);
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

                    unicodeString += toUTF<E>(sjisUnicodeTable[0x100 + ((c & 0x1F) << 8) | in[i++]]);
                    break;

                case 0xE:

                    if(i == in.size()) {
                        break;
                    }

                    unicodeString += toUTF<E>(sjisUnicodeTable[0x2100 + ((c & 0x1F) << 8) | in[i++]]);
                    break;

                default:
                    unicodeString += toUTF<E>(sjisUnicodeTable[c]);
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

                unicodeString += toUTF<E>(big5UnicodeTable[0xA1 + (((c - 0xA1) << 8) | in[i++])]);

            } else {

                unicodeString += toUTF<E>(big5UnicodeTable[c]);

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

                unicodeString += toUTF<E>(wansungUnicodeTable[0x100 + (((c - 0x21) << 8) | in[i++])]);

            } else {

                unicodeString += toUTF<E>(wansungUnicodeTable[c]);

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

                unicodeString += toUTF<E>(johabUnicodeTable[0x84 + (((c - 0x84) << 8) | in[i++])]);

            } else {

                unicodeString += toUTF<E>(johabUnicodeTable[c]);

            }

        }

        return unicodeString;

    }

}