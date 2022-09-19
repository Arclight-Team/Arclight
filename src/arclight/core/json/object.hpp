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

public:

	using StringType = Json::StringType;
	using Type = Json::Type;
	

	JsonObject();

	template<CC::JsonValue T>
	JsonObject(const StringType& name, const T& value) {
		emplace(name) = value;
	}


	JsonValue& emplace(const StringType& name);
	JsonValue& operator[](const StringType& name);

	JsonObject& insert(const StringType& name, const JsonValue& value);

	template<CC::JsonValue T>
	JsonObject& insert(const StringType& name, const T& value) {
		emplace(name) = value;
		return *this;
	}

	template<CC::JsonValue T>
	JsonObject& operator()(const StringType& name, const T& value) {
		return insert(name, value);
	}


	void clear();
	bool empty() const;

	ItemIterator begin();
	ItemConstIterator begin() const;
	ItemIterator end();
	ItemConstIterator end() const;

private:

	friend class JsonDocument;

	ItemContainer items;

};
