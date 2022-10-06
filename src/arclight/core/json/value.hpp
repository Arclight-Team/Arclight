/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 value.hpp
 */

#pragma once

#include "common.hpp"
#include "common/typetraits.hpp"
#include "stdext/any.hpp"



class JsonValue {
public:

	using StringType = Json::StringType;
	using Type = Json::Type;
	using IntegerType = Json::IntegerType;
	using FloatType = Json::FloatType;
	using Data = FastAny<TT::MaxType<StringType, FloatType, IntegerType, bool, JsonObject, JsonArray>>;

	constexpr JsonValue() noexcept : type(Type::None), integer(false) {}

	template<CC::JsonString T>
	constexpr JsonValue(const T& string) noexcept : type(Type::String), data(StringType(string)), integer(false) {}

	template<CC::JsonNumber T> requires(CC::Float<TT::RemoveCVRef<T>>)
	constexpr JsonValue(T number) noexcept : type(Type::Number), data(FloatType(number)), integer(false) {}

	template<CC::JsonNumber T> requires(CC::Integer<TT::RemoveCVRef<T>>)
	constexpr JsonValue(T number) noexcept : type(Type::Number), data(IntegerType(number)), integer(true) {}

	template<CC::JsonBoolean T>
	constexpr JsonValue(T boolean) noexcept : type(Type::Boolean), data(boolean), integer(false) {}

	constexpr JsonValue(std::nullptr_t) noexcept : type(Type::Null), integer(false) {}

	JsonValue(const JsonObject& object) noexcept;
	JsonValue(const JsonArray& array) noexcept;


	constexpr Type getType() const noexcept {
		return type;
	}

	template<CC::JsonNumber T>
	T toNumber() const {
		return safeCast<T>();
	}

	StringType toString() const;
	JsonObject toObject() const;
	JsonArray toArray() const;
	bool toBoolean() const;

	template<CC::JsonNumber T>
	T toNumber(T defaultValue) const {
		return defaultCast<T>(defaultValue);
	}

	StringType toString(const StringType& defaultValue) const;
	JsonObject toObject(const JsonObject& defaultValue) const;
	JsonArray toArray(const JsonArray& defaultValue) const;
	bool toBoolean(bool defaultValue) const;

	constexpr bool isString() const noexcept {
		return type == Type::String;
	}

	constexpr bool isNumber() const noexcept {
		return type == Type::Number;
	}

	constexpr bool isInteger() const noexcept {
		return isNumber() && integer;
	}

	constexpr bool isFloat() const noexcept {
		return isNumber() && !integer;
	}

	constexpr bool isObject() const noexcept {
		return type == Type::Object;
	}

	constexpr bool isArray() const noexcept {
		return type == Type::Array;
	}

	constexpr bool isBoolean() const noexcept {
		return type == Type::Boolean;
	}

	constexpr bool isNull() const noexcept {
		return type == Type::Null;
	}

private:

	template<CC::JsonValue Out>
	Out safeCast() const {
		
		constexpr Type targetType = Json::typeOf<Out>();

		// Implement string casting
		if (targetType != type) {
			throw JsonTypeCastException(type, targetType);
		}

		// Convert to appropriate numeric type
		if constexpr (targetType == Type::Number) {

			if (integer) {
				return static_cast<Out>(data.unsafeCast<IntegerType>());
			} else {
				return static_cast<Out>(data.unsafeCast<FloatType>());
			}

		} else {
			return data.unsafeCast<Out>();
		}

	}

	template<CC::JsonValue Value>
	Value defaultCast(const Value& v) const {

		constexpr Type targetType = Json::typeOf<Value>();

		// Implement string casting
		if (targetType != type) {
			return v;
		}

		// Convert to appropriate numeric type
		if constexpr (targetType == Type::Number) {

			if (integer) {
				return static_cast<Value>(data.unsafeCast<IntegerType>());
			} else {
				return static_cast<Value>(data.unsafeCast<FloatType>());
			}

		} else {
			return data.unsafeCast<Value>();
		}

	}

	Data data;
	Type type;
	bool integer;

};
