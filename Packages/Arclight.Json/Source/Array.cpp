/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Array.cpp
 */

#include "Json/Array.hpp"


JsonArray::JsonArray() {}



JsonValue& JsonArray::emplace() {
	return items.emplace_back();
}

void JsonArray::append(const JsonValue& value) {
	items.push_back(value);
}



JsonValue& JsonArray::operator[](SizeT index) {
	return items[index];
}

const JsonValue& JsonArray::operator[](SizeT index) const {
	return items[index];
}



auto JsonArray::begin() -> ItemIterator {
	return items.begin();
}

auto JsonArray::begin() const -> ItemConstIterator {
	return items.begin();
}

auto JsonArray::cbegin() const -> ItemConstIterator {
	return items.cbegin();
}

auto JsonArray::end() -> ItemIterator {
	return items.end();
}

auto JsonArray::end() const -> ItemConstIterator {
	return items.end();
}

auto JsonArray::cend() const -> ItemConstIterator {
	return items.cend();
}

auto JsonArray::rbegin() -> ItemReverseIterator {
	return items.rbegin();
}

auto JsonArray::rbegin() const -> ItemConstReverseIterator {
	return items.rbegin();
}

auto JsonArray::crbegin() const -> ItemConstReverseIterator {
	return items.crbegin();
}

auto JsonArray::rend() -> ItemReverseIterator {
	return items.rend();
}

auto JsonArray::rend() const -> ItemConstReverseIterator {
	return items.rend();
}

auto JsonArray::crend() const -> ItemConstReverseIterator {
	return items.crend();
}