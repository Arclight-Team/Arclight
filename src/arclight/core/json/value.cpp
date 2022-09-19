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



JsonValue::JsonValue(const JsonObject& object) noexcept : type(Type::Object), data(object), integer(false) {}

JsonValue::JsonValue(const JsonArray& array) noexcept : type(Type::Array), data(array), integer(false) {}



auto JsonValue::toString() const -> StringType {
	return safeCast<StringType>();
}

JsonObject JsonValue::toObject() const {
	return safeCast<JsonObject>();
}

JsonArray JsonValue::toArray() const {
	return safeCast<JsonArray>();
}

bool JsonValue::toBoolean() const {
	return safeCast<bool>();
}
