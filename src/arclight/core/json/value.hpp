/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 value.hpp
 */

#pragma once

#include "common.hpp"
#include "meta/typetraits.hpp"
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
	constexpr JsonValue(const T& string) : type(Type::String), data(StringType(string)), integer(false) {}

	template<CC::JsonNumber T> requires(CC::Float<TT::RemoveCVRef<T>>)
	constexpr JsonValue(T number) : type(Type::Number), data(FloatType(number)), integer(false) {}

	template<CC::JsonNumber T> requires(CC::Integer<TT::RemoveCVRef<T>>)
	constexpr JsonValue(T number) : type(Type::Number), data(IntegerType(number)), integer(true) {}

	template<CC::JsonBoolean T>
	constexpr JsonValue(T boolean) : type(Type::Boolean), data(boolean), integer(false) {}

	constexpr JsonValue(std::nullptr_t) noexcept : type(Type::Null), integer(false) {}

	template<class T, class U = TT::RemoveCVRef<T>> requires (CC::Equal<U, JsonArray> || CC::Equal<U, JsonObject>)
	JsonValue(T&& t) : type(CC::Equal<U, JsonArray> ? Type::Array : Type::Object), data(std::forward<T>(t)), integer(false) {}


	constexpr Type getType() const noexcept {
		return type;
	}

	template<CC::JsonNumber T>
	T toNumber() const {
		return safeCast<T>();
	}

	bool toBoolean() const;
	StringType toString() const;
	JsonArray& toArray();
	JsonObject& toObject();
	const JsonArray& toArray() const;
	const JsonObject& toObject() const;

	template<CC::JsonNumber T>
	T toNumber(T defaultValue) const {
		return defaultCast<T>(defaultValue);
	}

	bool toBoolean(bool defaultValue) const;
	StringType toString(const StringType& defaultValue) const;
	JsonArray& toArray(JsonArray& defaultValue);
	JsonObject& toObject(JsonObject& defaultValue);
	const JsonArray& toArray(const JsonArray& defaultValue) const;
	const JsonObject& toObject(const JsonObject& defaultValue) const;

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

	constexpr bool isValid() const noexcept {
		return type != Type::None;
	}

private:

	template<CC::JsonType Out>
	const Out& safeCastRef() const {

		constexpr Type targetType = Json::typeOf<Out>();

		if (targetType != type) {
			throw JsonTypeCastException(type, targetType);
		}

		return data.unsafeCast<Out>();

	}

	template<CC::JsonType Out>
	Out safeCast() const {

		constexpr Type targetType = Json::typeOf<Out>();

		if (targetType != type) {
			throw JsonTypeCastException(type, targetType);
		}

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

	template<CC::JsonType Value>
	const Value& defaultCastRef(const Value& v) const {

		constexpr Type targetType = Json::typeOf<Value>();

		if (targetType != type) {
			return v;
		}

		return data.unsafeCast<Value>();

	}

	template<CC::JsonType Value>
	Value defaultCast(const Value& v) const {

		constexpr Type targetType = Json::typeOf<Value>();

		if (targetType != type) {
			return v;
		}

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
