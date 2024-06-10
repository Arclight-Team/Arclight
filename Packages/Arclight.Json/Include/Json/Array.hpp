/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Array.hpp
 */

#pragma once

#include "Common.hpp"
#include "Object.hpp"

#include <vector>



class JsonArray {
private:

	using ItemContainer = std::vector<JsonValue>;
	using ItemIterator = ItemContainer::iterator;
	using ItemConstIterator = ItemContainer::const_iterator;
	using ItemReverseIterator = ItemContainer::reverse_iterator;
	using ItemConstReverseIterator = ItemContainer::const_reverse_iterator;

public:

	using StringType = Json::StringType;
	using Type = Json::Type;


	JsonArray();

	template<CC::JsonType T>
	JsonArray(std::initializer_list<T> list) {
		insert(list);
	}


	JsonValue& emplace();
	void append(const JsonValue& value);

	template<CC::JsonType T>
	void append(const T& value) {
		items.push_back(value);
	}

	template<CC::JsonType T>
	void insert(std::initializer_list<T> list) {

		for (SizeT i = 0; i < list.size(); i++)
			append(list.begin()[i]);

	}

	JsonValue& operator[](SizeT index);
	const JsonValue& operator[](SizeT index) const;


	constexpr void clear() {
		items.clear();
	}

	constexpr bool empty() const {
		return items.empty();
	}

	ItemIterator begin();
	ItemConstIterator begin() const;
	ItemConstIterator cbegin() const;

	ItemIterator end();
	ItemConstIterator end() const;
	ItemConstIterator cend() const;

	ItemReverseIterator rbegin();
	ItemConstReverseIterator rbegin() const;
	ItemConstReverseIterator crbegin() const;

	ItemReverseIterator rend();
	ItemConstReverseIterator rend() const;
	ItemConstReverseIterator crend() const;

private:

	ItemContainer items;

};
