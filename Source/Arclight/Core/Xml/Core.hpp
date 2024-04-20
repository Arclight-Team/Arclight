/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Core.hpp
 */

#pragma once

#include "Util/Bool.hpp"
#include "Meta/Concepts.hpp"
#include "Meta/TypeTraits.hpp"
#include "StdExt/OptionalRef.hpp"

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
	
	template<CC::Char CharType>
	class CharIterator;

	template<CC::Char CharType>
	class StringRef;


	template<CC::Char CharType>
	class Attribute;

	template<CC::Char CharType>
	class Node;

	template<CC::Char CharType>
	class Document;

#else

	class CharIterator;
	class StringRef;

	class Attribute;
	class Node;
	class Document;

#endif

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<CC::Char CharType>
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
