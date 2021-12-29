#pragma once

#include "xmlc.hpp"
#include "stringref.hpp"
#include "attribute.hpp"


namespace Xml
{
	
	enum class NodeType
	{
		Element,		// <...
		Attribute,		// ...=
		Value,			// ="..."
		Comment,		// <!-- ...
		Prolog,			// <?xml ...
		Doctype,		// <!DOCTYPE ...
		DTDDeclaration	// <!ELEMENT ...
	};

	
#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType = char>
#endif
	class Node
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

#ifdef XML_NODE_STORAGE_UNIFIED
		XML_TEMPLATE_INLINE Node(DocumentT& document, u64 parent, NodeType type) :
			owner(document),
			parent(parent),
			type(type),
			index(0)
		{}
#else
		XML_TEMPLATE_INLINE Node(DocumentT& document, OptionalRef<NodeT> parent, NodeType type) :
			owner(document),
			parent(parent),
			type(type)
		{}
#endif

		// Removes children, attributes, name and value
		XML_TEMPLATE_INLINE void clear() {

			children.clear();
			attributes.clear();

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

		constexpr NodeType getType() const {
			return type;
		}

		constexpr void setType(NodeType type) {
			this->type = type;
		}

		// Returns the document
		XML_TEMPLATE_INLINE DocumentT& getDocument() {
			return owner;
		}

		// Returns the document
		XML_TEMPLATE_INLINE const DocumentT& getDocument() const {
			return owner;
		}

		// Returns the parent node
		XML_TEMPLATE_INLINE OptionalRef<NodeT> getParent() {
			return parent;
		}

		// Returns the parent node
		XML_TEMPLATE_INLINE OptionalRef<const NodeT> getParent() const {
			return parent;
		}

		// Returns the next node
		XML_TEMPLATE_INLINE OptionalRef<NodeT> getNext() {

			if (!parent)
				return {};

			SizeT nextID = index + 1;

			if (nextID >= parent->children.size())
				return {};

			return *parent->children.at(nextID);

		}

		// Returns the next node
		XML_TEMPLATE_INLINE OptionalRef<const NodeT> getNext() const {

			if (!parent)
				return {};

			SizeT nextID = index + 1;

			if (nextID >= parent->children.size())
				return {};

			return *parent->children.at(nextID);

		}

		// Returns the previous node
		XML_TEMPLATE_INLINE OptionalRef<NodeT> getPrevious() {

			if (!parent)
				return {};

			SizeT prevID = index - 1;

			if (prevID >= parent->children.size())
				return {};

			return *parent->children.at(prevID);

		}

		// Returns the previous node
		XML_TEMPLATE_INLINE OptionalRef<const NodeT> getPrevious() const {

			if (!parent)
				return {};

			SizeT prevID = index - 1;

			if (prevID >= parent->children.size())
				return {};

			return *parent->children.at(prevID);

		}

		// Returns the first node with the given name
		XML_TEMPLATE_INLINE OptionalRef<NodeT> getNodeByName(const StringView& sv) {

			for (auto& node : children) {
				if (node->name == sv) {
					return *node;
				}
			}

			return {};

		}

		// Returns the first node with the given name
		XML_TEMPLATE_INLINE OptionalRef<const NodeT> getNodeByName(const StringView& sv) const {

			for (auto& node : children) {
				if (node->name == sv) {
					return *node;
				}
			}

			return {};

		}

		// Returns the first attribute with the given name
		XML_TEMPLATE_INLINE OptionalRef<AttributeT> getAttributeByName(const StringView& sv) {

			for (auto& attr : attributes) {
				if (attr->name == sv) {
					return *attr;
				}
			}

			return {};

		}

		// Returns the first attribute with the given name
		XML_TEMPLATE_INLINE OptionalRef<const AttributeT> getAttributeByName(const StringView& sv) const {

			for (auto& attr : attributes) {
				if (attr->name == sv) {
					return *attr;
				}
			}

			return {};

		}

		// Returns the first child node
		XML_TEMPLATE_INLINE OptionalRef<NodeT> getFirstChild() {

			if (children.empty())
				return {};

			return *children.front();

		}

		// Returns the first child node
		XML_TEMPLATE_INLINE OptionalRef<const NodeT> getFirstChild() const {

			if (children.empty())
				return {};

			return *children.front();

		}

		// Returns the first attribute
		XML_TEMPLATE_INLINE OptionalRef<AttributeT> getFirstAttribute() {

			if (attributes.empty())
				return {};

			return *attributes.front();

		}

		// Returns the first attribute
		XML_TEMPLATE_INLINE OptionalRef<const AttributeT> getFirstAttribute() const {

			if (attributes.empty())
				return {};

			return *attributes.front();

		}

		// Creates a children node
		XML_TEMPLATE_INLINE NodeT& create() {

			auto& node = children.emplace_back(std::make_unique<NodeT>(owner, *this, NodeType::Element));
			node->index = children.size() - 1;

			return *node;

		}

		// Creates an attribute
		XML_TEMPLATE_INLINE AttributeT& createAttribute() {

			auto& attr = attributes.emplace_back(std::make_unique<AttributeT>(*this));
			attr->index = attributes.size() - 1;

			return *attr;

		}

		Node(const NodeT&) = delete;
		Node& operator=(const NodeT&) = delete;

	private:

		XML_TEMPLATE_INLINE static void writeOpeningSequence(std::basic_ostream<CharType>& os, NodeType type) {

			switch (type) {

			case NodeType::Element:
				os << CharType('<');
				break;

			case NodeType::Prolog:
				os << CharType('<');
				os << CharType('?');
				break;

			case NodeType::Doctype:
			case NodeType::DTDDeclaration:
				os << CharType('<');
				os << CharType('!');
				break;

				//case NodeType::Data:
				//	break;

			case NodeType::Comment:
				os << CharType('<');
				os << CharType('!');
				os << CharType('-');
				os << CharType('-');
				break;

			default:
				break;

			}

		}

		XML_TEMPLATE_INLINE static void writeClosingSequence(std::basic_ostream<CharType>& os, NodeType type) {

			switch (type) {

			case NodeType::Element:
			case NodeType::Doctype:
			case NodeType::DTDDeclaration:
				os << CharType('>');
				break;

			case NodeType::Prolog:
				os << CharType('?');
				os << CharType('>');
				break;

				//case Xml::NodeType::Data:
				//	break;

			case NodeType::Comment:
				os << CharType('-');
				os << CharType('-');
				os << CharType('>');
				break;

			default:
				break;

			}

		}

#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		friend class Document;

#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
#endif
		friend class Attribute;


#ifdef XML_TEMPLATE_CHAR_TYPE
		template<Char CharType>
		friend std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>&, const Node<CharType>&);
#else
		friend std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>&, const Node&);
#endif

		DocumentT& owner;

		NodeType type;
#ifdef XML_NODE_STORAGE_UNIFIED
		u64 parent;
		u64 index;
		std::vector<u64> children;
		std::vector<u64> attributes;
#else
		OptionalRef<NodeT> parent;
		SizeT index;
		std::vector<std::unique_ptr<NodeT>> children;
		std::vector<std::unique_ptr<AttributeT>> attributes;
#endif

		StringRefT name;
		StringRefT value;

	};

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	Document<CharType>& Attribute<CharType>::getDocument() {
#else
	Document& Attribute::getDocument() {
#endif
		return parent.owner;
	}

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	const Document<CharType>& Attribute<CharType>::getDocument() const {
#else
	const Document& Attribute::getDocument() const {
#endif
		return parent.owner;
	}

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	OptionalRef<Attribute<CharType>> Attribute<CharType>::getNext() {
#else
	OptionalRef<Attribute> Attribute::getNext() {
#endif

		SizeT nextID = index + 1;

		if (nextID >= parent.attributes.size())
			return {};

		return *parent.attributes.at(nextID);

	}

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	OptionalRef<const Attribute<CharType>> Attribute<CharType>::getNext() const {
#else
	OptionalRef<const Attribute> Attribute::getNext() const {
#endif

		SizeT nextID = index + 1;

		if (nextID >= parent.attributes.size())
			return {};

		return *parent.attributes.at(nextID);

	}

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	OptionalRef<Attribute<CharType>> Attribute<CharType>::getPrevious() {
#else
	OptionalRef<Attribute> Attribute::getPrevious() {
#endif

		SizeT prevID = index - 1;

		if (prevID >= parent.attributes.size())
			return {};

		return *parent.attributes.at(prevID);

	}

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	OptionalRef<const Attribute<CharType>> Attribute<CharType>::getPrevious() const {
#else
	OptionalRef<const Attribute> Attribute::getPrevious() const {
#endif

		SizeT prevID = index - 1;

		if (prevID >= parent.attributes.size())
			return {};

		return *parent.attributes.at(prevID);
	
	}

#ifdef XML_TEMPLATE_CHAR_TYPE
	template<Char CharType>
	std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, const Node<CharType>& node) {
#else
	std::basic_ostream<CharType>& operator<<(std::basic_ostream<CharType>& os, const Node& node) {
#endif

		node.writeOpeningSequence(os, node.type);

		// Write node body string
		switch (node.type) {

		case NodeType::Element:
		case NodeType::Prolog:
		case NodeType::Doctype:
		case NodeType::DTDDeclaration:
			os << node.name;
			break;

		//case NodeType::Data:
		case NodeType::Comment:
			os << node.value;
			break;

		default:
			break;

		}

		// Write node attributes
		for (auto attr = node.getFirstAttribute(); attr; attr = attr->getNext()) {

			os << CharType(' ');
			os << *attr;

		}

		node.writeClosingSequence(os, node.type);

		return os;

	}

}
