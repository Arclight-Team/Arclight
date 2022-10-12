/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 object.cpp
 */

#include "object.hpp"
#include "document.hpp"



bool JsonObject::contains(const StringType& name) const {
	return items.contains(name);
}

JsonValue& JsonObject::operator[](const StringType& name) {
	return items[name];
}

const JsonValue& JsonObject::operator[](const StringType& name) const {

	if (!contains(name)) {
		return nullValue;
	} else {
		return items.at(name);
	}

}

void JsonObject::insert(const StringType& name, const JsonValue& value) {
	items.insert_or_assign(name, value);
}



void JsonObject::clear() {
	items.clear();
}

bool JsonObject::empty() const {
	return items.empty();
}

auto JsonObject::begin() -> ItemIterator {
	return items.begin();
}

auto JsonObject::begin() const -> ItemConstIterator {
	return items.begin();
}

auto JsonObject::cbegin() const -> ItemConstIterator {
	return items.cbegin();
}

auto JsonObject::end() -> ItemIterator {
	return items.end();
}

auto JsonObject::end() const -> ItemConstIterator {
	return items.end();
}

auto JsonObject::cend() const -> ItemConstIterator {
	return items.cend();
}

auto JsonObject::rbegin() -> ItemReverseIterator {
	return items.rbegin();
}

auto JsonObject::rbegin() const -> ItemConstReverseIterator {
	return items.rbegin();
}

auto JsonObject::crbegin() const -> ItemConstReverseIterator {
	return items.crbegin();
}

auto JsonObject::rend() -> ItemReverseIterator {
	return items.rend();
}

auto JsonObject::rend() const -> ItemConstReverseIterator {
	return items.rend();
}

auto JsonObject::crend() const -> ItemConstReverseIterator {
	return items.crend();
}



JsonObject JsonObject::fromJson(const StringView& json) {
	return JsonDocument(json).getRoot();
}

auto JsonObject::toJson(const JsonObject& object, bool compact) -> StringType {
	return JsonDocument(object).write(compact);
}

auto JsonObject::toJson(bool compact) -> StringType {
	return toJson(*this, compact);
}
