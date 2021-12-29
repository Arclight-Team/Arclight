/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 xmlc.hpp
 */

#pragma once

#include "util/bool.hpp"
#include "util/concepts.hpp"
#include "util/typetraits.hpp"
#include "stdext/optionalref.hpp"

#include <stdexcept>
#include <memory>
#include <string>
#include <vector>


#define XML_CHAR_FILTER_LUT
#define XML_TEMPLATE_CHAR_TYPE

#ifdef XML_TEMPLATE_CHAR_TYPE
#define XML_TEMPLATE_INLINE
#else
#define XML_TEMPLATE_INLINE inline
#endif


namespace Xml
{

#ifndef XML_TEMPLATE_CHAR_TYPE
	using CharType = char;
#endif

	class XmlParseError : public std::runtime_error
	{
	public:

		XmlParseError(const std::string& s) : std::runtime_error(s) {}

	};

#ifdef XML_TEMPLATE_CHAR_TYPE
	
	template<Char CharType>
	class CharIterator;

	template<Char CharType>
	class StringRef;


	template<Char CharType>
	class Attribute;

	template<Char CharType>
	class Node;

	template<Char CharType>
	class Document;

#else

	class CharIterator;
	class StringRef;

	class Attribute;
	class Node;
	class Document;

#endif

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
#endif
	constexpr CharType selectQuoteChar(const std::basic_string_view<CharType>& sv) {

		const CharType sq = CharType('\'');
		const CharType dq = CharType('\"');

		for (auto ch : sv) {

			if (ch == sq) return dq;
			if (ch == dq) return sq;

		}

		return dq;

	};

}
