/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Exception.inl
 */

#include "Common/Exception.hpp"
#include "Util/String.hpp"



class JsonException : public ArclightException
{
public:
	using ArclightException::ArclightException;

	virtual const char* name() const noexcept override { return "Json Exception"; }

};



class JsonValueNotFoundException : public JsonException
{
public:

	explicit JsonValueNotFoundException(const std::string& valueName) noexcept :
		JsonException(String::format("Value '%s' not found", valueName.c_str())) {}

	virtual const char* name() const noexcept override { return "Json Value Not Found Exception"; }

};



class JsonTypeCastException : public JsonException
{
public:

	using JsonException::JsonException;

	JsonTypeCastException() noexcept : JsonException("Error while performing type cast") {}

	explicit JsonTypeCastException(Json::Type source, Json::Type target) noexcept :
		JsonTypeCastException(String::format("Invalid type cast from %s to %s", Json::typeName(source), Json::typeName(target))) {}

	virtual const char* name() const noexcept override { return "Json Type Cast Exception"; }

};


class JsonSyntaxError : public JsonException
{
public:

	using JsonException::JsonException;
	JsonSyntaxError() noexcept : JsonException("Syntax error while parsing JSON data") {}

	virtual const char* name() const noexcept override { return "Json Syntax Error"; }

};
