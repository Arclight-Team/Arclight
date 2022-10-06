/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 object.hpp
 */

#pragma once

#include "common.hpp"
#include "value.hpp"
#include <map>



class JsonObject {
private:

	using ItemContainer = std::map<Json::StringType, JsonValue>;
	using ItemIterator = ItemContainer::iterator;
	using ItemConstIterator = ItemContainer::const_iterator;
	using ItemReverseIterator = ItemContainer::reverse_iterator;
	using ItemConstReverseIterator = ItemContainer::const_reverse_iterator;

public:

	using StringType = Json::StringType;
	using Type = Json::Type;
	

	JsonObject() = default;

	template<CC::JsonValue T>
	JsonObject(const StringType& name, const T& value) {
		emplace(name, value);
	}

	bool contains(const StringType& name) const;

	template<class... Args>
	JsonValue& emplace(const StringType& name, Args&&... args) {
		return *items.try_emplace(name, std::forward<Args>(args)...).first;
	}

	template<class... Args>
	JsonValue& emplace(StringType&& name, Args&&... args) {
		return *items.try_emplace(std::move(name), std::forward<Args>(args)...).first;
	}

	JsonValue& operator[](const StringType& name);
	const JsonValue& operator[](const StringType& name) const;

	void insert(const StringType& name, const JsonValue& value);

	template<CC::JsonValue T>
	JsonObject& insert(const StringType& name, const T& value) {
		return emplace(name, value);
	}

	template<CC::JsonValue T>
	JsonObject& operator()(const StringType& name, const T& value) {
		return insert(name, value);
	}


	void clear();
	bool empty() const;

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

	inline static JsonValue nullValue;

	friend class JsonDocument;

	ItemContainer items{};

};
