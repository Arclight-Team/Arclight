/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 array.cpp
 */

#include "array.hpp"


JsonArray::JsonArray() {}



JsonValue& JsonArray::emplace() {
	return items.emplace_back();
}

void JsonArray::append(const JsonValue& value) {
	items.push_back(value);
}



JsonValue& JsonArray::operator[](SizeT index) {
	return items.at(index);
}

const JsonValue& JsonArray::operator[](SizeT index) const {
	return items.at(index);
}
