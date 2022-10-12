/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 array.hpp
 */

#pragma once

#include "common.hpp"
#include "object.hpp"
#include <vector>
#include "util/log.hpp"



class JsonArray {
private:

	using ItemContainer = std::vector<JsonValue>;
	using ItemIterator = ItemContainer::iterator;
	using ItemConstIterator = ItemContainer::const_iterator;

public:

	using StringType = Json::StringType;
	using Type = Json::Type;


	JsonArray();

	template<CC::JsonValue T>
	JsonArray(std::initializer_list<T> list) {
		insert(list);
	}


	JsonValue& emplace();
	void append(const JsonValue& value);

	template<CC::JsonValue T>
	void append(const T& value) {
		items.push_back(value);
	}

	template<CC::JsonValue T>
	void insert(std::initializer_list<T> list) {

		for (SizeT i = 0; i < list.size(); i++)
			append(list.begin()[i]);

	}

	template<CC::JsonValue... Args>
	JsonArray& operator()(Args&&... args) {
		(append(std::forward<Args>(args)), ...);
		return *this;
	}

	JsonValue& operator[](SizeT index);
	const JsonValue& operator[](SizeT index) const;
	

	constexpr void clear() {
		items.clear();
	}

	constexpr bool empty() const {
		return items.empty();
	}


	constexpr ItemIterator begin() {
		return items.begin();
	}

	constexpr ItemConstIterator begin() const {
		return items.begin();
	}

	constexpr ItemIterator end() {
		return items.end();
	}

	constexpr ItemConstIterator end() const {
		return items.end();
	}

private:

	ItemContainer items;

};
