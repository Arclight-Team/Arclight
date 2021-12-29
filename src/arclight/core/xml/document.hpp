/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 document.hpp
 */

#pragma once

#include "xmlc.hpp"
#include "charfilter.hpp"
#include "chariterator.hpp"
#include "stringref.hpp"
#include "attribute.hpp"
#include "node.hpp"

#include <sstream>


namespace Xml
{
	
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType = char>
#endif
	class Document
	{
	private:

		using String		= std::basic_string<CharType>;
		using StringView	= std::basic_string_view<CharType>;

#ifdef XML_TEMPLATE_CHAR_TYPE
		using FiltersT		= Filters<CharType>;
		using StringRefT	= StringRef<CharType>;
		using CharIteratorT = CharIterator<CharType>;
		using AttributeT	= Attribute<CharType>;
		using NodeT			= Node<CharType>;
		using DocumentT		= Document<CharType>;
#else
		using FiltersT		= Filters;
		using StringRefT	= StringRef;
		using CharIteratorT = CharIterator;
		using AttributeT	= Attribute;
		using NodeT			= Node;
		using DocumentT		= Document;
#endif

	public:

		Document() :
#ifdef XML_NODE_STORAGE_UNIFIED
			root(*this, UINT64_MAX, NodeType::Element)
#else
			root(*this, {}, NodeType::Element)
#endif
		{}

		XML_TEMPLATE_INLINE SizeT read(const StringView& sv) {

			CharIteratorT it(sv);
			auto beg = it;

			if (it.cmp({ CharType(0xEF), CharType(0xBB), CharType(0xBF) }))
				it += 3;

			while (it.valid()) {

				if (!it.skip(FiltersT::Space))
					break;

				// New node
				if (it.cmp('<')) {

					auto& node = root.create();

					// Skip opening bracket
					++it;

#ifdef XML_NODE_STORAGE_UNIFIED
					readNode(it, node.index);
#else
					readNode(it, node);
#endif
				}

			}

			if (it.overflow())
				throw XmlParseError("Text data iterator overflown");

			return &it - &beg;

		}

		XML_TEMPLATE_INLINE void write(std::basic_ostream<CharType>& os, int indentWidth = 4, CharType indentChar = ' ') const {

			writeNodes(os, root.getFirstChild(), indentWidth, indentChar);

		}

		XML_TEMPLATE_INLINE void write(String& s, int indentWidth = 4, CharType indentChar = ' ') const {
		
			std::ostringstream os;

			write(os, indentWidth, indentChar);

			s = os.str();
		
		}

		XML_TEMPLATE_INLINE NodeT& getRoot() {
			return root;
		}

		XML_TEMPLATE_INLINE const NodeT& getRoot() const {
			return root;
		}

	private:

		XML_TEMPLATE_INLINE void writeIndent(std::basic_ostream<CharType>& os, int indentLevel, int indentWidth = 4, CharType indentChar = ' ') const {

			os << String(indentLevel * indentWidth, indentChar);

		}

		XML_TEMPLATE_INLINE void writeAttributes(std::basic_ostream<CharType>& os, OptionalRef<const NodeT> node, int indentLevel = 0) const {

			if (!node)
				return;

			for (auto attr = node->getFirstAttribute(); attr; attr = attr->getNext()) {

				os << CharType(' ');
				os << *attr;

			}

		}

		XML_TEMPLATE_INLINE void writeNodes(std::basic_ostream<CharType>& os, OptionalRef<const NodeT> node, int indentWidth = 4, CharType indentChar = ' ', int indentLevel = 0) const {

			for (; node; node = node->getNext()) {

				auto child = node->getFirstChild();

				StringRef body;

				// Get node body string
				switch (node->type) {

				case NodeType::Element:
				case NodeType::Prolog:
				case NodeType::Doctype:
				case NodeType::DTDDeclaration:
					body = node->name;
					break;

				//case NodeType::Data:
				case NodeType::Comment:
					body = node->value;
					break;

				default:
					break;

				}

				// Indent the current line
				writeIndent(os, indentLevel, indentWidth, indentChar);

				// Insert the opening sequence
				NodeT::writeOpeningSequence(os, node->type);

				os << body;

				// Insert attributes
				writeAttributes(os, node, indentLevel);

				// Insert a self-closing element
				if (node->type == Xml::NodeType::Element && !child) {
					os << CharType('/');
					os << CharType('>');
					os << CharType('\n');
				}
				else {
					NodeT::writeClosingSequence(os, node->type);
					os << CharType('\n');
				}

				// Write children nodes
				writeNodes(os, child, indentWidth, indentChar, indentLevel + 1);

				// Close the element
				if (node->type == Xml::NodeType::Element && child) {

					writeIndent(os, indentLevel, indentWidth, indentChar);

					os << CharType('<');
					os << body;
					os << CharType('>');
					os << CharType('\n');

				}
			}

		}

		XML_TEMPLATE_INLINE StringRefT readAttributeName(CharIteratorT& it) {

			StringRefT s;

			// Skip leading space characters
			if (!it.skip(FiltersT::Space)) {
				throw XmlParseError("Text data interrupted");
			}

			s.begin(&it);

			// Skip string characters
			if (!it.skip(FiltersT::AttributeName)) {
				throw XmlParseError("Text data interrupted");
			}

			s.end(&it);

			// Skip trailing space characters
			if (!it.skip(FiltersT::Space)) {
				throw XmlParseError("Text data interrupted");
			}

			return s;

		}

		XML_TEMPLATE_INLINE StringRefT readAttributeValue(CharIteratorT& it) {

			StringRefT s;

			// Skip leading space characters
			if (!it.skip(FiltersT::Space)) {
				throw XmlParseError("Text data interrupted");
			}

			// Check opening quote character
			if (!it.cmp(FiltersT::Quote)) {
				throw XmlParseError("Expected a single quote or double quote character");
			}

			// Read opening quote character
			CharType quoteCh = *it++;

			s.begin(&it);

			// Skip string characters
			if (!it.skip(quoteCh)) {
				throw XmlParseError("Text data interrupted");
			}

			s.end(&it++);

			// Skip trailing space characters
			if (!it.skip(FiltersT::Space)) {
				throw XmlParseError("Text data interrupted");
			}

			return s;

		}

#ifdef XML_NODE_STORAGE_UNIFIED
		XML_TEMPLATE_INLINE void readAttributes(CharIteratorT& it, u64 uuid, const std::initializer_list<CharType>& closingSequence) {

			auto& node = getNode(uuid).get();
#else
		XML_TEMPLATE_INLINE void readAttributes(CharIteratorT & it, NodeT& node, const std::initializer_list<CharType>& closingSequence) {
#endif

			while (it.valid()) {

				// Check if node is being closed
				if (it.cmp(closingSequence)) {
					return;
				}

				auto& attr = node.createAttribute();				

				// Parse attribute name
				attr.name = readAttributeName(it);

				if (!it.cmp('=')) {
					throw XmlParseError("Expected '=' character in attribute definition");
				}

				// Parse attribute value
				attr.value = readAttributeValue(++it);

			}

			throw XmlParseError("Unexpected end of data");

		}

#ifdef XML_NODE_STORAGE_UNIFIED
		XML_TEMPLATE_INLINE bool readProlog(CharIteratorT& it, u64 uuid) {

			auto& node = getNode(uuid).get();

#else
		XML_TEMPLATE_INLINE bool readProlog(CharIteratorT & it, NodeT& node) {
#endif

			if (it.cmp(FiltersT::Space)) {
				throw XmlParseError("Expected prolog name");
			}

			node.type = NodeType::Prolog;
			node.name.begin(&it);

			if (!it.skip(FiltersT::Name)) {
				throw XmlParseError("Text data interrupted");
			}

			node.name.end(&it);

			// Parse attributes

#ifdef XML_NODE_STORAGE_UNIFIED
			readAttributes(it, node.index, { '?', '>' });
#else
			readAttributes(it, node, { '?', '>' });
#endif

			it += 2;

			return true;

		}

#ifdef XML_NODE_STORAGE_UNIFIED
		XML_TEMPLATE_INLINE bool readDTDDecl(CharIteratorT& it, u64 uuid) {

			auto& node = getNode(uuid).get();

#else
		XML_TEMPLATE_INLINE bool readDTDDecl(CharIteratorT & it, NodeT& node) {
#endif

			if (it.cmp(FiltersT::Space)) {
				throw XmlParseError("Expected prolog name");
			}

			node.type = NodeType::DTDDeclaration;
			node.name.begin(&it);

			if (!it.skip(FiltersT::Name)) {
				throw XmlParseError("Text data interrupted");
			}

			node.name.end(&it);

			if (node.name == "DOCTYPE")
				node.type = NodeType::Doctype;

			// Read single attribute name (no value)
			auto& attr = node.createAttribute();

			attr.name = readAttributeName(it);

			while (it.valid()) {

				if (it.cmp('>')) {
					++it;

					return true;
				}

				++it;
			}

			throw XmlParseError("Expected closing bracket");

			return false;

		}

#ifdef XML_NODE_STORAGE_UNIFIED
		XML_TEMPLATE_INLINE bool readComment(CharIteratorT& it, u64 uuid) {

			auto& node = getNode(uuid).get();

#else
		XML_TEMPLATE_INLINE bool readComment(CharIteratorT & it, NodeT& node) {
#endif

			node.type = NodeType::Comment;
			node.value.begin(&it);

			while (it.valid()) {

				// Check for nested comments
				if (it.cmp({ '<', '!', '-', '-' })) {
					throw XmlParseError("Unexpected nested comment opening");
				}

				// Check for closing sequence
				if (it.cmp({ '-', '-', '>' })) {

					node.value.end(&it);

					it += 3;

					return true;
				}

				it++;
			}

			throw XmlParseError("Unterminated comment");

		}

#ifdef XML_NODE_STORAGE_UNIFIED
		XML_TEMPLATE_INLINE void readElement(CharIteratorT& it, u64 uuid) {

			auto& node = getNode(uuid).get();

#else
		XML_TEMPLATE_INLINE void readElement(CharIteratorT& it, NodeT& node) {
#endif

			if (it.cmp(FiltersT::Space)) {
				throw XmlParseError("Expected element name");
			}

			node.type = NodeType::Element;
			node.name.begin(&it);

			if (!it.skip(FiltersT::Name)) {
				throw XmlParseError("Text data interrupted");
			}

			node.name.end(&it);

			// Parse attributes
			bool selfClosing = false;

			while (it.valid()) {
				
				// Check if element is self-closing
				if (it.cmp('/')) {
					++it;

					selfClosing = true;
				}

				// Check if node is being closed
				if (it.cmp('>')) {
					break;
				}
				else if (selfClosing) {
					throw XmlParseError("Unexpected self-closing token in element");
				}

				auto& attr = node.createAttribute();

				// Parse attribute name
				attr.name = readAttributeName(it);

				if (!it.cmp('=')) {
					throw XmlParseError("Expected '=' character in attribute definition");
				}

				// Parse attribute value
				attr.value = readAttributeValue(++it);

			}

			++it;

			if (selfClosing) {
				return;
			}

			if (!it.valid()) {
				throw XmlParseError("Unexpected end of data");
			}

			// Parse element body (value and child elements)

			while (it.valid()) {

				auto vbeg = it;

				// Skip spaces
				if (!it.skip(FiltersT::Space))
					throw XmlParseError("Text data interrupted");

				if (it.cmp('<')) {

					// Element node closing
					if (it.cmp('/', 1)) {
						it += 2;

						StringRefT name;

						name.begin(&it);

						if (!it.skip(FiltersT::Name))
							throw XmlParseError("Text data interrupted");

						name.end(&it);

						if (name != node.name)
							throw XmlParseError("Invalid closing element name");

						if (!it.cmp('>'))
							throw XmlParseError("Expected closing bracket");

						++it;

						return;
					}

					// Child element node
					auto& child = node.create();


#ifdef XML_NODE_STORAGE_UNIFIED
					readElement(++it, child.index);
#else
					readElement(++it, child);
#endif
				}
				else {
					// Rewind iterator and parse value
					it = vbeg;

					node.value.begin(&it);

					while (it.valid()) {

						if (it.cmp('<')) {
							node.value.end(&it);

							break;
						}

						++it;
					}
				}
			}

			if (!it.valid()) {
				throw XmlParseError("Unexpected end of data");
			}

		}

#ifdef XML_NODE_STORAGE_UNIFIED
		bool readNode(CharIteratorT& it, u64 uuid) {

			switch (*it++) {

			case CharType('<'):
				throw XmlParseError("Error: <");

			case CharType('?'):
				return readProlog(it, uuid);

			case CharType('!'):
				
				if (it.cmp({ '-', '-' }))
					return readComment(it += 2, uuid);

				return readDTDDecl(it, uuid);

			default:
				readElement(--it, uuid);
				return true;

			}

			return true;

		}
#else
		XML_TEMPLATE_INLINE bool readNode(CharIteratorT& it, NodeT& node) {

			switch (*it++) {

			case CharType('<'):
				throw XmlParseError("Error: <");

			case CharType('?'):
				return readProlog(it, node);

			case CharType('!'):
				
				if (it.cmp({ '-', '-' }))
					return readComment(it += 2, node);

				return readDTDDecl(it, node);

			default:
				readElement(--it, node);
				return true;

			}

			return true;

		}
#endif

	private:

#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		friend class Node;

#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		friend class Attribute;

#ifdef XML_NODE_STORAGE_UNIFIED

		Node& createNode();

		Attribute& createAttribute();

		OptionalRef<Node> getNode(u64 uuid);

		OptionalRef<const Node> getNode(u64 uuid) const;

		OptionalRef<Attribute> getAttribute(u64 uuid);

		OptionalRef<const Attribute> getAttribute(u64 uuid) const;

		std::vector<Node> nodeStorage;
		std::vector<Attribute> attributeStorage;
		std::vector<u64>  nodeUUIDStorage;
		std::vector<u64>  attributeUUIDStorage;
		u64 nodeUUID = 0;
		u64 attributeUUID = 0;

#endif

		NodeT root;

	};

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, const Document<CharType>& document) {
#else
	std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, const Document& document) {
#endif

		document.write(os);

		return os;

	}

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	std::basic_istream<CharType>& operator>>(std::basic_istream<CharType>& is, Document<CharType>& document) {
#else
	std::basic_istream<CharType>& operator>>(std::basic_istream<CharType>& is, Document& document) {
#endif

		std::basic_string<CharType> s(std::istreambuf_iterator<CharType>(is), {});

		SizeT read = document.read(s);

		return is.seekg(read);
	
	}

}
