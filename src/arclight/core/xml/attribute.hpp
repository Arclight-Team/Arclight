/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 attribute.hpp
 */

#pragma once

#include "xmlc.hpp"
#include "stringref.hpp"


namespace Xml
{
	
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType = char>
#endif
	class Attribute
	{
	private:

		using String		= std::basic_string<CharType>;
		using StringView	= std::basic_string_view<CharType>;
		
#ifdef XML_TEMPLATE_CHAR_TYPE
		using StringRefT	= StringRef<CharType>;
		using AttributeT	= Attribute<CharType>;
		using NodeT			= Node<CharType>;
		using DocumentT		= Document<CharType>;
#else
		using StringRefT	= StringRef;
		using AttributeT	= Attribute;
		using NodeT			= Node;
		using DocumentT		= Document;
#endif

	public:

		Attribute() = default;
#ifdef XML_NODE_STORAGE_UNIFIED
		XML_TEMPLATE_INLINE Attribute(DocumentT& document, u64 parent) :
			owner(document), 
			parent(parent),
			index(0)
		{}
#else
		XML_TEMPLATE_INLINE Attribute(NodeT& parent) :
			parent(parent)
		{}
#endif

		// Clear name and value
		constexpr void clear() {

			name.clear();
			value.clear();

		}

		constexpr auto& getName() {
			return name;
		}

		constexpr const auto& getName() const {
			return name;
		}

		constexpr void setName(const StringView& sv) {
			name = sv;
		}

		constexpr auto& getValue() {
			return value;
		}

		constexpr const auto& getValue() const {
			return value;
		}

		constexpr void setValue(const StringView& sv) {
			value = sv;
		}

		// Returns the document
		XML_TEMPLATE_INLINE DocumentT& getDocument();

		// Returns the document
		XML_TEMPLATE_INLINE const DocumentT& getDocument() const;

		// Returns the parent node
		XML_TEMPLATE_INLINE NodeT& getParent() {
			return parent;
		}

		// Returns the parent node
		XML_TEMPLATE_INLINE const NodeT& getParent() const {
			return parent;
		}

		// Returns the next attribute
		XML_TEMPLATE_INLINE OptionalRef<AttributeT> getNext();

		// Returns the next attribute
		XML_TEMPLATE_INLINE OptionalRef<const AttributeT> getNext() const;

		// Returns the previous attribute
		XML_TEMPLATE_INLINE OptionalRef<AttributeT> getPrevious();

		// Returns the previous attribute
		XML_TEMPLATE_INLINE OptionalRef<const AttributeT> getPrevious() const;

	private:

#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		friend class Document;

#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		friend class Node;


#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
		friend std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>&, const Attribute<CharType>&);
#else
		friend std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>&, const Attribute&);
#endif

#ifdef XML_NODE_STORAGE_UNIFIED
		DocumentT& owner;

		u64 parent;
		u64 index;
#else
		NodeT& parent;
		SizeT index;
#endif
		
		StringRefT name;
		StringRefT value;

	};

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	XML_TEMPLATE_INLINE std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, const Attribute<CharType>& attribute) {
#else
	XML_TEMPLATE_INLINE std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, const Attribute& attribute) {
#endif

		if (attribute.name.empty())
			return os;

		os << attribute.name;

		if (attribute.value.empty())
			return os;

		auto quoteCh = selectQuoteChar(attribute.value);

		os << CharType('=');
		os << quoteCh;
		os << attribute.value;
		os << quoteCh;

		return os;

	}

}
