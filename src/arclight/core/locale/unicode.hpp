#pragma once

#include "util/string.hpp"
#include "types.hpp"

#include <iterator>
#include <string>



namespace Unicode {
    
    enum class Encoding {
        UTF8,
        UTF16LE,
        UTF16BE,
        UTF32LE,
        UTF32BE,
        UTF16 = LittleEndian ? UTF16LE : UTF16BE,
        UTF32 = LittleEndian ? UTF32LE : UTF32BE
    };

    using enum Unicode::Encoding;

    template<Encoding E>
    consteval ByteOrder byteOrderFromEncoding() noexcept {

        if(E == UTF16LE || E == UTF32LE) {
            return ByteOrder::Little;
        } else if (E == UTF16BE || E == UTF32BE) {
            return ByteOrder::Big;
        } else {
            return ByteOrder::Little;
        }

    }

    template<Encoding E>
    consteval bool requiresEndianConversion() noexcept {

        if (E != UTF8) {
            return Bits::requiresEndianConversion(byteOrderFromEncoding<E>());
        } else {
            return false;
        }

    }

    template<Encoding E>
    consteval bool isUTF8() noexcept {
        return E == UTF8;
    }

    template<Encoding E>
    consteval bool isUTF16() noexcept {
        return E == UTF16LE || E == UTF16BE;
    }

    template<Encoding E>
    consteval bool isUTF32() noexcept {
        return E == UTF32LE || E == UTF32BE;
    }

    template<Unicode::Encoding E> struct UTFEncodingTraits {};

    template<> struct UTFEncodingTraits<Unicode::UTF8>    { using Type = char8_t;  constexpr static SizeT MaxDecomposed = 4; };
    template<> struct UTFEncodingTraits<Unicode::UTF16LE> { using Type = char16_t; constexpr static SizeT MaxDecomposed = 2; };
    template<> struct UTFEncodingTraits<Unicode::UTF16BE> { using Type = char16_t; constexpr static SizeT MaxDecomposed = 2; };
    template<> struct UTFEncodingTraits<Unicode::UTF32LE> { using Type = char32_t; constexpr static SizeT MaxDecomposed = 1; };
    template<> struct UTFEncodingTraits<Unicode::UTF32BE> { using Type = char32_t; constexpr static SizeT MaxDecomposed = 1; };

    using Codepoint = char32_t;


    //Codepoint to UTF
    template<Encoding E, class T = typename UTFEncodingTraits<E>::Type>
    constexpr static SizeT toUTF(Codepoint codepoint, T* p) noexcept {

        if constexpr (isUTF8<E>()) {

            if(codepoint < 0x80) {

                p[0] = codepoint;
                return 1;

            } else if (codepoint < 0x800) {

                p[0] = (codepoint >> 6) | 0xC0;
                p[1] = (codepoint & 0x3F) | 0x80;
                return 2;

            } else if (codepoint < 0x10000) {

                p[0] = (codepoint >> 12) | 0xE0;
                p[1] = (codepoint >> 6 & 0x3F) | 0x80;
                p[2] = (codepoint & 0x3F) | 0x80;
                return 3;

            } else {

                p[0] = (codepoint >> 18) | 0xF0;
                p[1] = (codepoint >> 12 & 0x3F) | 0x80;
                p[2] = (codepoint >> 6 & 0x3F) | 0x80;
                p[3] = (codepoint & 0x3F) | 0x80;
                return 4;

            }

        } else if constexpr (isUTF16<E>()) {

            constexpr bool convert = requiresEndianConversion<E>();

            if (codepoint >= 0x10000) {

                T lowSurrogate = 0xDC00 + codepoint & 0x3FF;
                T highSurrogate = 0xD800 + (codepoint >> 10);

                if constexpr (convert) {
                    lowSurrogate = Bits::swap16(lowSurrogate);
                    highSurrogate = Bits::swap16(highSurrogate);
                }

                p[0] = highSurrogate;
                p[1] = lowSurrogate;
                return 2;

            } else {

                T c = codepoint;

                if constexpr (convert) {
                    c = Bits::swap16(c);
                }

                p[0] = c;
                return 1;

            }

        } else if constexpr (isUTF32<E>()) {

            constexpr bool convert = requiresEndianConversion<E>();

            if constexpr (convert) {
                codepoint = Bits::swap32(codepoint);
            }

            p[0] = codepoint;
            return 1;

        }

    }

    //UTF to codepoint
    template<Encoding E, class T = typename UTFEncodingTraits<E>::Type>
    constexpr Codepoint getCodepoint(const T* p, SizeT& count) noexcept {

        if constexpr (isUTF8<E>()) {

            T c0 = p[0];

            if(c0 < 0x80) {

                count = 1;
                return c0;

            } else if (c0 < 0xE0) {

                count = 2;
                return (c0 & 0x1F) << 6 | p[1] & 0x3F;

            } else if (c0 < 0xF0) {

                count = 3;
                return (c0 & 0x0F) << 12 | (p[1] & 0x3F) << 6 | p[2] & 0x3F;

            } else {

                count = 4;
                return (c0 & 0x07) << 18 | (p[1] & 0x3F) << 12 | (p[2] & 0x3F) << 6 | p[3] & 0x3F;

            }

        } else if constexpr (isUTF16<E>()) {

            constexpr bool convert = requiresEndianConversion<E>();

            T c0 = p[0];

            if constexpr (convert) {
                c0 = Bits::swap16(c0);
            }

            if ((c0 & 0xD800) == 0xD800) {

                T c1 = p[1];

                if constexpr (convert) {
                    c1 = Bits::swap16(c1);
                }

                //Assume we have a low-surrogate in c1
                count = 2;
                return (c0 & 0x3FF) << 10 | (c1 & 0x3FF);

            } else {

                count = 1;
                return c0;

            }

        } else if constexpr (isUTF32<E>()) {

            constexpr bool convert = requiresEndianConversion<E>();

            T c = p[0];

            if constexpr (convert) {
                c = Bits::swap32(c);
            }

            count = 1;
            return c;

        }

    }

    //UTF to codepoint
    template<Encoding E, class T = typename UTFEncodingTraits<E>::Type>
    constexpr Codepoint getCodepoint(const T* p) noexcept {

        [[maybe_unused]] SizeT count;
        return getCodepoint<E>(p, count);

    }

    //UTF encoded size of next codepoint
    template<Encoding E, class T = typename UTFEncodingTraits<E>::Type>
    constexpr u32 getEncodedSize(const T* p) noexcept {

        if constexpr (isUTF8<E>()) {

            T c = *p;

            if((c & 0xF0) == 0xF0) {
                return 4;
            } else if((c & 0xE0) == 0xE0) {
                return 3;
            } else if((c & 0xC0) == 0xC0) {
                return 2;
            } else {
                return 1;
            }

        } else if constexpr (isUTF16<E>()) {

            T c = *p;

            if((c & 0xF8) == 0xD8) {
                return 2;
            } else {
                return 1;
            }

        } else {

            return 1;

        }

    }

    //UTF encoded size of previous codepoint
    template<Encoding E, class T = typename UTFEncodingTraits<E>::Type>
    constexpr u32 getEncodedSizeBackwards(const T* p) noexcept {

        if constexpr (isUTF8<E>()) {

            if(!(*(p - 1) & 0x80)) {
                return 1;
            } else if((*(p - 2) & 0xC0) == 0xC0) {
                return 2;
            } else if((*(p - 3) & 0xE0) == 0xE0) {
                return 3;
            } else {
                return 4;
            }

        } else if constexpr (isUTF16<E>()) {

            T c = *(p - 1);

            if((c & 0xF8) == 0xD8) {
                return 2;
            } else {
                return 1;
            }

        } else {

            return 1;

        }

    }

}