/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 exception.inl
 */

#include "util/string.hpp"
#include <string>
#include <stdexcept>



class JsonTypeCastException : public std::runtime_error
{
public:

	JsonTypeCastException() noexcept : JsonTypeCastException("Error while performing type cast") {}
	explicit JsonTypeCastException(const std::string& msg) noexcept : std::runtime_error(msg) {}
	explicit JsonTypeCastException(Json::Type source, Json::Type target) noexcept : 
		JsonTypeCastException(String::format("Invalid type cast from %s to %s", Json::typeName(source), Json::typeName(target))) {}

};


class JsonSyntaxError : public std::runtime_error
{
public:

	JsonSyntaxError() noexcept : JsonSyntaxError("Syntax error while parsing JSON data") {}
	explicit JsonSyntaxError(const std::string& msg) noexcept : std::runtime_error(msg) {}

};
