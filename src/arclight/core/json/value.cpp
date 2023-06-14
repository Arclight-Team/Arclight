/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 value.cpp
 */

#include "value.hpp"
#include "array.hpp"
#include "object.hpp"

#include <utility>



bool JsonValue::toBoolean() const {
	return safeCast<bool>();
}

auto JsonValue::toString() const -> StringType {
	return safeCast<StringType>();
}

JsonArray& JsonValue::toArray() {
	return const_cast<JsonArray&>(std::as_const(*this).toArray());
}

JsonObject& JsonValue::toObject() {
	return const_cast<JsonObject&>(std::as_const(*this).toObject());
}

const JsonArray& JsonValue::toArray() const {
	return safeCastRef<JsonArray>();
}

const JsonObject& JsonValue::toObject() const {
	return safeCastRef<JsonObject>();
}

bool JsonValue::toBoolean(bool defaultValue) const {
	return defaultCast<bool>(defaultValue);
}

auto JsonValue::toString(const StringType& defaultValue) const -> StringType {
	return defaultCast<StringType>(defaultValue);
}

JsonArray& JsonValue::toArray(JsonArray& defaultValue) {
	return const_cast<JsonArray&>(std::as_const(*this).toArray(defaultValue));
}

JsonObject& JsonValue::toObject(JsonObject& defaultValue) {
	return const_cast<JsonObject&>(std::as_const(*this).toObject(defaultValue));
}

const JsonArray& JsonValue::toArray(const JsonArray& defaultValue) const {
	return defaultCastRef<JsonArray>(defaultValue);
}

const JsonObject& JsonValue::toObject(const JsonObject& defaultValue) const {
	return defaultCastRef<JsonObject>(defaultValue);
}