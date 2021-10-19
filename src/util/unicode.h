#pragma once

#include <iterator>
#include <string>

#include "util/string.h"
#include "types.h"




namespace Unicode {
    
    enum class Encoding {
        UTF8,
        UTF16LE,
        UTF16BE,
        UTF32LE,
        UTF32BE
    };

    using enum Unicode::Encoding;


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
    constexpr static u32 toCodepoint(const std::string_view& in, u32& readBytes) noexcept {

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

    template<Encoding E>
    constexpr static u32 toCodepoint(const std::string_view& in) noexcept {
        u32 dummy;
        return toCodepoint<E>(in, dummy);
    }


}




template<Unicode::Encoding E>
class UnicodeIterator {

public:

    using difference_type   = std::ptrdiff_t;
    using value_type        = u32;
    using pointer           = u32;
    using reference         = u32;


    constexpr UnicodeIterator(const std::string_view& s, SizeT start = 0) noexcept : str(s), pos(start) {

        if(pos > s.size()) {
            pos = s.size();
        }

    }

    constexpr UnicodeIterator(const UnicodeIterator& it) noexcept = default;

    constexpr reference operator*() const noexcept {return getCodepoint();}
    constexpr pointer operator->() const noexcept {return getCodepoint();}

    constexpr UnicodeIterator& operator++() noexcept {advance(); return *this;}
    constexpr UnicodeIterator operator++(int) noexcept {auto cpy = *this; ++(*this); return cpy;}
    constexpr UnicodeIterator& operator--() noexcept {retreat(); return *this;}
    constexpr UnicodeIterator operator--(int) noexcept {auto cpy = *this; --(*this); return cpy;}

    constexpr auto operator==(const UnicodeIterator& other) const noexcept {
        return str == other.str && pos == other.pos;
    }

private:

    constexpr u32 getCodepoint() const noexcept {
        return Unicode::toCodepoint<E>(str.substr(pos));
    }

    constexpr void advance() noexcept {

        if constexpr (E == Unicode::Encoding::UTF8) {

            u8 c = str[pos];

            if((c & 0xF0) == 0xF0) {
                pos += 4;
            } else if((c & 0xE0) == 0xE0) {
                pos += 3;
            } else if((c & 0xC0) == 0xC0) {
                pos += 2;
            } else {
                pos += 1;
            }

        } else if constexpr (E == Unicode::Encoding::UTF16LE || E == Unicode::Encoding::UTF16BE) {

            constexpr static bool convert = Bits::requiresEndianConversion(E == Unicode::Encoding::UTF16LE ? ByteOrder::Little : ByteOrder::Big);

            u8 c = str[pos + !convert];

            if((c & 0xF8) == 0xD8) {
                pos += 4;
            } else {
                pos += 2;
            }

        } else {

            pos += 4;

        }

    }

    constexpr void retreat() noexcept {

        if constexpr (E == Unicode::Encoding::UTF8) {

           if(!(str[pos - 1] & 0x80)) {
               pos -= 1;
           } else if((str[pos - 2] & 0xC0) == 0xC0) {
               pos -= 2;
           } else if((str[pos - 3] & 0xE0) == 0xE0) {
               pos -= 3;
           } else {
               pos -= 4;
           }

        } else if constexpr (E == Unicode::Encoding::UTF16LE || E == Unicode::Encoding::UTF16BE) {

            constexpr static bool convert = Bits::requiresEndianConversion(E == Unicode::Encoding::UTF16LE ? ByteOrder::Little : ByteOrder::Big);

            u8 c = str[pos - 1 - convert];

            if((c & 0xF8) == 0xD8) {
                pos -= 4;
            } else {
                pos -= 2;
            }

        } else {

            pos -= 4;

        }

    }

    std::string_view str;
    SizeT pos;

};

template<Unicode::Encoding E>
using UnicodeReverseIterator = std::reverse_iterator<UnicodeIterator<E>>;



namespace Unicode {

    template<Encoding E>
    constexpr UnicodeIterator<E> begin(const std::string& str) noexcept {
        return UnicodeIterator<E>(str);
    }

    template<Encoding E>
    constexpr UnicodeIterator<E> end(const std::string& str) noexcept {
        return UnicodeIterator<E>(str, str.size());
    }

    template<Encoding E>
    constexpr UnicodeIterator<E> cbegin(const std::string& str) noexcept {
        return begin(str);
    }

    template<Encoding E>
    constexpr UnicodeIterator<E> cend(const std::string& str) noexcept {
        return end(str);
    }

    template<Encoding E>
    constexpr UnicodeReverseIterator<E> rbegin(const std::string& str) noexcept {
        return UnicodeReverseIterator<E>(str);
    }

    template<Encoding E>
    constexpr UnicodeReverseIterator<E> rend(const std::string& str) noexcept {
        return UnicodeReverseIterator<E>(str, str.size());
    }

    template<Encoding E>
    constexpr UnicodeReverseIterator<E> crbegin(const std::string& str) noexcept {
        return rbegin(str);
    }

    template<Encoding E>
    constexpr UnicodeReverseIterator<E> crend(const std::string& str) noexcept {
        return rend(str);
    }

}