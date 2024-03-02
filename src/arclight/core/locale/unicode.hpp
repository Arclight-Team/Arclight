/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 unicode.hpp
 */

#pragma once

#include "util/bits.hpp"
#include "util/string.hpp"
#include "common/exception.hpp"
#include "common/types.hpp"

#include <string>
#include <span>



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

		if (E == UTF16LE || E == UTF32LE) {
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

	template<Encoding E0, Encoding E1>
	consteval bool isEquivalentEncoding() noexcept {
		return (isUTF8<E0>() && isUTF8<E1>()) || (isUTF16<E0>() && isUTF16<E1>()) || (isUTF32<E0>() && isUTF32<E1>());
	}



	template<CC::Char C, bool FlipEndianess = false>
	consteval Encoding fromCharType() noexcept {

		constexpr SizeT s = sizeof(C);

		if constexpr (s == 2) {

			if constexpr (FlipEndianess) {
				return LittleEndian ? Encoding::UTF16BE : Encoding::UTF16LE;
			} else {
				return Encoding::UTF16;
			}

		} else if constexpr (s == 4) {

			if constexpr (FlipEndianess) {
				return LittleEndian ? Encoding::UTF32BE : Encoding::UTF32LE;
			} else {
				return Encoding::UTF32;
			}

		} else {

			return Encoding::UTF8;

		}

	};

	template<CC::Char C> struct TypedChar { using Type = C; };
	template<> struct TypedChar<char> { using Type = char8_t; };
	template<> struct TypedChar<unsigned char> { using Type = char8_t; };
	template<> struct TypedChar<signed char> { using Type = char8_t; };
	template<> struct TypedChar<wchar_t> { using Type = TT::Conditional<sizeof(wchar_t) == 1, char8_t, TT::Conditional<sizeof(wchar_t) == 2, char16_t, char32_t>>; };


	template<CC::Char C, bool FlipEndianess = false>
	constexpr Encoding TypeEncoding = fromCharType<C, FlipEndianess>();

	template<Unicode::Encoding E> struct UTFEncodingTraits {};

	template<> struct UTFEncodingTraits<Unicode::UTF8>    { using Type = char8_t;  constexpr static SizeT MaxDecomposed = 4; constexpr static SizeT MinUnitSize = 1; };
	template<> struct UTFEncodingTraits<Unicode::UTF16LE> { using Type = char16_t; constexpr static SizeT MaxDecomposed = 2; constexpr static SizeT MinUnitSize = 2; };
	template<> struct UTFEncodingTraits<Unicode::UTF16BE> { using Type = char16_t; constexpr static SizeT MaxDecomposed = 2; constexpr static SizeT MinUnitSize = 2; };
	template<> struct UTFEncodingTraits<Unicode::UTF32LE> { using Type = char32_t; constexpr static SizeT MaxDecomposed = 1; constexpr static SizeT MinUnitSize = 4; };
	template<> struct UTFEncodingTraits<Unicode::UTF32BE> { using Type = char32_t; constexpr static SizeT MaxDecomposed = 1; constexpr static SizeT MinUnitSize = 4; };

	using Codepoint = char32_t;


	template<Encoding E, CC::Char T>
	consteval bool isCharEncodable() {
		return sizeof(T) == sizeof(typename UTFEncodingTraits<E>::Type);
	}


	constexpr u8 utf8SequenceSize[256] = {
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1
	};


	//UTF encoded size of next codepoint
	template<Encoding E, CC::Char T>
	constexpr SizeT getEncodedSize(const T* p) noexcept requires (isCharEncodable<E, T>()) {

		if constexpr (isUTF8<E>()) {

			u8 c = *p;
			return utf8SequenceSize[c];

		} else if constexpr (isUTF16<E>()) {

			T c = *p;

			if ((c & 0xF800) != 0xD800) {
				return 1;
			} else {
				return 2;
			}

		} else {

			return 1;

		}

	}

	//UTF encoded size of previous codepoint
	template<Encoding E, CC::Char T>
	constexpr SizeT getEncodedSizeBackwards(const T* p) noexcept requires (isCharEncodable<E, T>()) {

		if constexpr (isUTF8<E>()) {

			if ((*(p - 1) & 0x80) == 0x00) {
				return 1;
			} else if ((*(p - 2) & 0xC0) == 0xC0) {
				return 2;
			} else if ((*(p - 3) & 0xE0) == 0xE0) {
				return 3;
			} else {
				return 4;
			}

		} else if constexpr (isUTF16<E>()) {

			T c = *(p - 1);

			if ((c & 0xF800) != 0xD800) {
				return 1;
			} else {
				return 2;
			}

		} else {

			return 1;

		}

	}

	//UTF decoded size of codepoint
	template<Encoding E>
	constexpr SizeT getDecodedSize(Codepoint cp) noexcept {

		if constexpr (isUTF8<E>()) {

			if (cp < 0x80) {
				return 1;
			} else if (cp < 0x800) {
				return 2;
			} else if (cp < 0x10000) {
				return 3;
			} else {
				return 4;
			}

		} else if constexpr (isUTF16<E>()) {

			if (cp < 0x10000) {
				return 1;
			} else {
				return 2;
			}

		} else {

			return 1;

		}

	}

	//Codepoint to UTF
	template<Encoding E, CC::Char T>
	constexpr SizeT encode(Codepoint codepoint, T* p) noexcept requires (isCharEncodable<E, T>()) {

		if constexpr (isUTF8<E>()) {

			if (codepoint < 0x80) {

				p[0] = codepoint & 0xFF;
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

				T c = codepoint & 0xFFFF;

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
	template<Encoding E, CC::Char T>
	constexpr Codepoint decode(const T* p, SizeT& count) noexcept requires (isCharEncodable<E, T>()) {

		if constexpr (isUTF8<E>()) {

			u8 c0 = p[0];

			if (c0 < 0x80) {

				count = 1;
				return c0;

			} else if (c0 < 0xE0) {

				count = 2;
				return Codepoint(c0 & 0x1F) << 6 | p[1] & 0x3F;

			} else if (c0 < 0xF0) {

				count = 3;
				return Codepoint(c0 & 0x0F) << 12 | Codepoint(p[1] & 0x3F) << 6 | p[2] & 0x3F;

			} else {

				count = 4;
				return Codepoint(c0 & 0x07) << 18 | Codepoint(p[1] & 0x3F) << 12 | Codepoint(p[2] & 0x3F) << 6 | p[3] & 0x3F;

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
				return Codepoint(c0 & 0x3FF) << 10 | (c1 & 0x3FF);

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
	template<Encoding E, CC::Char T>
	constexpr Codepoint decode(const T* p) noexcept requires (isCharEncodable<E, T>()) {

		[[maybe_unused]] SizeT count;
		return decode<E>(p, count);

	}

	//UTF to UTF
	template<Encoding From, Encoding To, CC::Char SrcT, CC::Char DstT>
	constexpr SizeT transcode(const SrcT* s, DstT* d, SizeT& decoded) noexcept requires (isCharEncodable<From, SrcT>() && isCharEncodable<To, DstT>()) {

		if constexpr (From == To) {

			SizeT count = getEncodedSize<From>(s);
			std::copy(s, s + count, d);
			decoded = count;

			return count;

		} else {

			return Unicode::encode<To>(Unicode::decode<From>(s, decoded), d);

		}

	}

	template<Encoding From, Encoding To, CC::Char SrcT, CC::Char DstT>
	constexpr SizeT transcode(const SrcT* s, DstT* d) noexcept requires (isCharEncodable<From, SrcT>() && isCharEncodable<To, DstT>()) {

		[[maybe_unused]] SizeT decodedCPs;
		return transcode<From, To>(s, d, decodedCPs);

	}

	//Returns the size of the codepoint encoded in From if converted to To
	template<Encoding From, Encoding To, CC::Char T>
	constexpr SizeT getTranscodedSize(const T* p) noexcept requires (isCharEncodable<From, T>()) {

		if constexpr (isEquivalentEncoding<From, To>()) {
			return getEncodedSize<From>(p);
		} else {
			return getDecodedSize<To>(decode<From>(p));
		}

	}


	template<CC::Char SrcCharT, Unicode::Encoding Src, CC::Char DestCharT, Unicode::Encoding Dest>
	constexpr std::basic_string<DestCharT> convert(std::basic_string_view<SrcCharT> src) {

		constexpr SizeT SrcMinUnitSize = Unicode::UTFEncodingTraits<Src>::MinUnitSize;
		constexpr SizeT DestMinUnitSize = Unicode::UTFEncodingTraits<Dest>::MinUnitSize;
		constexpr SizeT AssemblyUnits = SrcMinUnitSize / sizeof(SrcCharT);

		using SrcUnit = typename Unicode::UTFEncodingTraits<Src>::Type;

		//Destination char unit mismatch
		if constexpr (DestMinUnitSize != sizeof(DestCharT)) {
			throw UnsupportedOperationException("Alternative destination char encodings not yet supported");
		}

		//Calculate the new string size
		SizeT size = 0;
		SizeT offset = 0;

		while (offset < src.size()) {

			//Input needs to be assembled in advance
			if constexpr (SrcMinUnitSize > sizeof(SrcCharT)) {

				//Bad assembly, cancel
				if (offset + AssemblyUnits > src.size()) {
					break;
				}

				SrcUnit units[Unicode::UTFEncodingTraits<Src>::MaxDecomposed];
				units[0] = Bits::assemble<SrcUnit>(src.data() + offset);

				SizeT count = Unicode::getEncodedSize<Src>(units);
				SizeT margin = count * AssemblyUnits;

				if (offset + margin > src.size()) {
					//Bad unicode coding, stop the conversion here
					break;
				}

				for (SizeT i = 1; i < count; i++) {
					units[i] = Bits::assemble<SrcUnit>(src.data() + offset + i * AssemblyUnits);
				}

				size += Unicode::getTranscodedSize<Src, Dest>(units);
				offset += margin;

			} else {

				SizeT margin = Unicode::getEncodedSize<Src>(src.data() + offset);

				if (offset + margin > src.size()) {
					//Bad unicode coding, stop the conversion here
					break;
				}

				size += Unicode::getTranscodedSize<Src, Dest>(src.data() + offset);
				offset += margin;

			}

		}

		//Convert
		std::basic_string<DestCharT> convertedString(size, 0);
		SizeT decoded = 0;
		SizeT offsetIn = 0;
		SizeT offsetOut = 0;

		while (offsetIn < src.size()) {

			//Input needs to be assembled in advance
			if constexpr (SrcMinUnitSize > sizeof(SrcCharT)) {

				SrcUnit units[Unicode::UTFEncodingTraits<Src>::MaxDecomposed];
				units[0] = Bits::assemble<SrcUnit>(src.data() + offsetIn);

				SizeT count = Unicode::getEncodedSize<Src>(units);
				SizeT margin = count * AssemblyUnits;

				for (SizeT i = 1; i < count; i++) {
					units[i] = Bits::assemble<SrcUnit>(src.data() + offsetIn + i * AssemblyUnits);
				}

				offsetIn += margin;
				offsetOut += Unicode::transcode<Src, Dest>(units, convertedString.data() + offsetOut);

			} else {

				Codepoint cp = Unicode::decode<Src>(src.data() + offsetIn, decoded);
				offsetIn += decoded;
				offsetOut += Unicode::encode<Dest>(cp, convertedString.data() + offsetOut);

			}

		}

		return convertedString;

	}

	template<Unicode::Encoding Src, Unicode::Encoding Dest>
	constexpr std::string convertString(std::wstring_view src) {
		return convert<wchar_t, Src, char, Dest>(src);
	}

	template<Unicode::Encoding Src, Unicode::Encoding Dest>
	constexpr std::wstring convertString(std::string_view src) {
		return convert<char, Src, wchar_t, Dest>(src);
	}

}