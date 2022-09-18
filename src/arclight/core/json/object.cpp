/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 object.cpp
 */

#include "object.hpp"



JsonObject::JsonObject() {}



JsonValue& JsonObject::emplace(const StringType& name) {
	return items[name];
}

JsonValue& JsonObject::operator[](const StringType& name) {
	return emplace(name);
}

JsonObject& JsonObject::insert(const StringType& name, const JsonValue& value) {
	emplace(name) = value;
	return *this;
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

auto JsonObject::end() -> ItemIterator {
	return items.end();
}

auto JsonObject::end() const -> ItemConstIterator {
	return items.end();
}
