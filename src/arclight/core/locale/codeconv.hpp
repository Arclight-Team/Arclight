/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 codeconv.hpp
 */

#pragma once

#include "unicode.hpp"
#include "util/string.hpp"
#include "util/bits.hpp"
#include "types.hpp"

#include <string>



namespace CodeConv {

    using Unicode::Encoding;
	using Unicode::Codepoint;
    
    extern const Codepoint sjisUnicodeTable[0x3100];
    extern const Codepoint big5UnicodeTable[0x5FA1];
    extern const Codepoint wansungUnicodeTable[0x5E00];
    extern const Codepoint johabUnicodeTable[0x7C84];


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

                    //unicodeString += Unicode::toUTF<E>(sjisUnicodeTable[0x100 + ((c & 0x1F) << 8) | in[i++]]);
                    break;

                case 0xE:

                    if(i == in.size()) {
                        break;
                    }

                    //unicodeString += Unicode::toUTF<E>(sjisUnicodeTable[0x2100 + ((c & 0x1F) << 8) | in[i++]]);
                    break;

                default:
                    //unicodeString += Unicode::toUTF<E>(sjisUnicodeTable[c]);
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

                //unicodeString += Unicode::toUTF<E>(big5UnicodeTable[0xA1 + (((c - 0xA1) << 8) | in[i++])]);

            } else {

                //unicodeString += Unicode::toUTF<E>(big5UnicodeTable[c]);

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

                //unicodeString += Unicode::toUTF<E>(wansungUnicodeTable[0x100 + (((c - 0x21) << 8) | in[i++])]);

            } else {

                //unicodeString += Unicode::toUTF<E>(wansungUnicodeTable[c]);

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

                //unicodeString += Unicode::toUTF<E>(johabUnicodeTable[0x84 + (((c - 0x84) << 8) | in[i++])]);

            } else {

                //unicodeString += Unicode::toUTF<E>(johabUnicodeTable[c]);

            }

        }

        return unicodeString;

    }

}