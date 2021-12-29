/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 stringref.hpp
 */

#pragma once

#include "xmlc.hpp"


namespace Xml
{
	
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType = char>
#endif
	class StringRef
	{
	private:

		using String		= std::basic_string<CharType>;
		using StringView	= std::basic_string_view<CharType>;
		using CharTraits	= std::char_traits<CharType>;

	public:

		constexpr StringRef() :
			string(nullptr),
			length(0)
		{}

		constexpr StringRef(const CharType* c) :
			string(c),
			length(CharTraits::length(c))
		{}

		constexpr StringRef(const StringView& sv) :
			string(sv.data()),
			length(sv.size())
		{}


		constexpr void clear() {
			string = nullptr;
			length = 0;
		}


		constexpr StringView toStringView() const {
			return { string, length };
		}

		constexpr String toString() const {
			return { string, length };
		}


		constexpr void size(SizeT size) {
			length = size;
		}

		constexpr void begin(const CharType* data) {
			string = data;
		}

		constexpr void end(const CharType* end) {
			length = end - string;
		}


		constexpr SizeT size() const {
			return length;
		}

		constexpr const CharType* data() const {
			return string;
		}

		constexpr const CharType* begin() const {
			return string;
		}

		constexpr const CharType* end() const {
			return string + length;
		}



		constexpr bool empty() const {
			return !size() || !data();
		}

		constexpr operator bool() const {
			return !empty();
		}

		constexpr bool operator==(const CharType* str) const {

			if (CharTraits::length(str) != length)
				return false;

			for (u32 i = 0; i < length; i++) {

				if (string[i] != str[i])
					return false;

			}

			return true;

		}

		constexpr bool operator==(const String& str) const {

			if (str.length() != length)
				return false;

			for (u32 i = 0; i < length; i++) {

				if (string[i] != str[i])
					return false;

			}

			return true;

		}


		constexpr bool operator==(const StringView& str) const {

			if (str.length() != length)
				return false;

			for (u32 i = 0; i < length; i++) {

				if (string[i] != str[i])
					return false;

			}

			return true;

		}

	private:

		const CharType* string;
		SizeT length;

	};

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	constexpr CharType selectQuoteChar(const StringRef<CharType>& string) {
#else
	constexpr CharType selectQuoteChar(const StringRef& string) {
#endif
		return selectQuoteChar(string.toStringView());
	}

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	XML_TEMPLATE_INLINE std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, const StringRef<CharType>& string) {
#else
	XML_TEMPLATE_INLINE std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, const StringRef& string) {
#endif
		if (!string)
			return os;

		return os << string.toStringView();
	}

}
