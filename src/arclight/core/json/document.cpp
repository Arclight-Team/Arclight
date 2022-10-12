/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 document.cpp
 */

#include "document.hpp"
#include "filesystem/path.hpp"
#include "filesystem/file.hpp"



JsonDocument::JsonDocument(const JsonObject& root) : root(root) {}

JsonDocument::JsonDocument(const StringView& json) {
	read(json);
}



void JsonDocument::read(const StringView& json) {

	clear();

	Iterator it = { json.cbegin(), json.cend() };

	for (; it.cur < it.end; it.cur++) {

		switch (it.cur[0]) {

		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;

		case '{':
			readObject(it, root);
			break;

		}

	}

}

auto JsonDocument::write(bool compact) -> StringType {

	StringType string;

	writeObject(string, root, compact, 1);

	return string;

}



void JsonDocument::clear() {
	root.clear();
}

bool JsonDocument::empty() const {
	return root.empty();
}



void JsonDocument::setRoot(const JsonObject& root) {
	this->root = root;
}

const JsonObject& JsonDocument::getRoot() {
	return root;
}



JsonDocument JsonDocument::fromFile(const Path& path) {

	File file(path, File::Text | File::In);
	return JsonDocument(file.readAllText());

}



bool JsonDocument::readComment(Iterator& it) {

	if (it.cur[0] != '/' || it.rem() < 2)
		return false;
		
	if (it.cur[1] != '/' && it.cur[1] != '*')
		return false;

	bool singleLine = it.cur[1] == '/';

	for (it.cur += 2; it.cur < it.end; it.cur++) {

		if (singleLine) {
			
			if (it.matchAny("\r\n")) {
				it.cur += (it.cur[0] == '\r') ? 2 : 1;
				return true;
			}
			
		} else {

			if (it.matchSequence("*/")) {
				it.cur += 2;
				return true;
			}

		}

	}

	// Single-line comments can be interrupted by an EOF
	if (singleLine)
		return true;
	
	// Multi-line comments require a closing sequence
	throw JsonSyntaxError("Unteriminated multi-line comment found");

}

bool JsonDocument::readString(Iterator& it, StringType& string) {

	if (it.cur[0] != '"')
		return false;

	for (it.cur++; it.cur < it.end; it.cur++) {

		auto ch = it.cur[0];
		if (ch == '"')
			break;

		string += it.cur[0];

	}

	// Unterminated string
	return !it.overflown();

}

bool JsonDocument::readNumber(Iterator& it, JsonValue& value) {

	Iterator tmp = it;

	if (tmp.cur[0] == '-')
		tmp.cur++;

	bool integer = true;

	// Select the numeric type
	for (; tmp.cur < tmp.end; tmp.cur++) {

		if (!Character::isDigit(tmp.cur[0])) {

			// Digit sequence interrupted

			switch (tmp.cur[0]) {

			case '.': // Decimal point, use Json::FloatType
				integer = false;
				break;

			case 'e': // Scientific notation, use Json::FloatType
				integer = false;
				break;

			}

			break;

		}

	}

	std::istringstream is(StringType(it.cur, it.end));

	if (integer) {

		Json::IntegerType n;
		is >> n;
		value = n;

	}
	else {

		Json::FloatType f;
		is >> f;
		value = f;

	}

	it.cur += is.tellg();
	it.cur -= 1;

	return !is.fail();

}

void JsonDocument::readValue(Iterator& it, JsonValue& value) {

	for (; it.cur < it.end; it.cur++) {

		// Skip comments
		if (readComment(it))
			continue;

		auto ch = it.cur[0];

		switch (ch) {

		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;

		case '"': // String
		{
			StringType string;

			if (!readString(it, string))
				throw JsonSyntaxError("Unexpected EOF while reading string");

			value = string;
			return;
		}

		case '{': // Object
		{
			JsonObject object;

			readObject(it, object);

			value = object;
			return;
		}

		case '[': // Array
		{
			JsonArray array;

			readArray(it, array);

			value = array;
			return;
		}

		case 't': // true
		{
			if (!it.matchSequence("true"))
				throw JsonSyntaxError("Invalid symbol found in value definition");

			it.cur += 3;
			value = true;
			return;
		}

		case 'f': // false
		{
			if (!it.matchSequence("false"))
				throw JsonSyntaxError("Invalid symbol found in value definition");

			it.cur += 4;
			value = false;
			return;
		}

		case 'n': // null
		{
			if (!it.matchSequence("null"))
				throw JsonSyntaxError("Invalid symbol found in value definition");

			it.cur += 3;
			value = nullptr;
			return;
		}

		default: // Number

			if (ch == '-' || Character::isDigit(ch)) {

				if (readNumber(it, value))
					return;

			}

			throw JsonSyntaxError("Invalid symbol found in value definition");

		}

	}

}

bool JsonDocument::readName(Iterator& it, StringType& name, bool& closed) {

	closed = false;
	
	for (; it.cur < it.end; it.cur++) {

		// Skip comments
		if (readComment(it))
			continue;

		switch (it.cur[0]) {

		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;

		case '}': // Object closed
			return closed = true;

		case '"':
		{
			if (!readString(it, name))
				throw JsonSyntaxError("Unexpected EOF while reading string");

			return true;
		}

		}

	}

	return false;

}

bool JsonDocument::readComma(Iterator& it, char closingChar, bool& closed) {

	closed = false;

	for (; it.cur < it.end; it.cur++) {

		// Skip comments
		if (readComment(it))
			continue;

		auto ch = it.cur[0];
		if (ch == closingChar)
			return closed = true;

		switch (ch) {

		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;

		case ',':
			return true;

		default:
			return false;

		}

	}

	return false;

}

bool JsonDocument::readColon(Iterator& it) {

	for (; it.cur < it.end; it.cur++) {

		// Skip comments
		if (readComment(it))
			continue;

		switch (it.cur[0]) {

		case ' ':
		case '\t':
		case '\r':
		case '\n':
			break;

		case ':':
			return true;

		default:
			return false;

		}

	}

	return false;

}

void JsonDocument::readArray(Iterator& it, JsonArray& array) {

	u32 step = ReadStepValue;

	if (it.cur[0] == '[')
		it.cur++;

	for (; it.cur < it.end; it.cur++) {

		// Skip comments
		if (readComment(it))
			continue;

		switch (step) {

		case ReadStepValue:
		{
			JsonValue value;

			readValue(it, value);

			array.append(value);

			step++;
			break;
		}

		case ReadStepComma:
		{
			bool closed;

			if (!readComma(it, ']', closed))
				throw JsonSyntaxError("Expected a comma separator or a closing character");

			if (closed)
				return;

			step = ReadStepValue;
			break;
		}

		}

	}

}

void JsonDocument::readObject(Iterator& it, JsonObject& object) {

	JsonObject::ItemIterator item{};
	u32 step = ReadStepName;

	if (it.cur[0] == '{')
		it.cur++;

	for (; it.cur < it.end; it.cur++) {

		// Skip comments
		if (readComment(it))
			continue;

		switch (step) {

		case ReadStepName:
		{
			bool closed;
			StringType name;

			if (!readName(it, name, closed))
				throw JsonSyntaxError("Expected a member name definition");

			if (closed)
				return;

			auto it = object.items.try_emplace(name);
			if (!it.second)
				throw JsonSyntaxError("Duplicate member name found");

			item = it.first;
			step++;
			break;
		}

		case ReadStepColon:
		{
			if (!readColon(it))
				throw JsonSyntaxError("Expected a name:value separator character");

			step++;
			break;
		}

		case ReadStepValue:
		{
			JsonValue value;

			readValue(it, value);

			item->second = value;

			step++;
			break;
		}

		case ReadStepComma:
		{
			bool closed;

			if (!readComma(it, '}', closed))
				throw JsonSyntaxError("Expected a comma separator or a closing character");

			if (closed)
				return;

			step = ReadStepName;
			break;
		}

		}

	}

}



void JsonDocument::writeValue(StringType& string, const JsonValue& value, bool compact, u32 level) {

	switch (value.getType()) {

	case Json::Type::String:
		string += '"' + value.toString() + '"';
		break;

	case Json::Type::Number:
	{
		std::ostringstream os;

		if (value.isInteger()) {

			os << +value.toNumber<Json::IntegerType>();
			string += os.str();

		} else {

			os << value.toNumber<Json::FloatType>();
			string += os.str();

		}

		break;
	}

	case Json::Type::Object:
		writeObject(string, value.toObject(), compact, level + 1);
		break;

	case Json::Type::Array:
		writeArray(string, value.toArray(), compact, level + 1);
		break;

	case Json::Type::Boolean:
		string += value.toBoolean() ? "true" : "false";
		break;

	case Json::Type::Null:
		string += "null";
		break;

	}

}

void JsonDocument::writeArray(StringType& string, const JsonArray& array, bool compact, u32 level) {

	StringType lineTab(level * IndentationLevel, IndentationChar);
	StringType lastTab = "";

	if (!compact && !array.empty())
		lastTab = StringType((level - 1) * IndentationLevel, IndentationChar);

	string += "[";

	bool first = true;

	for (const auto& value : array) {

		if (first) {
			first = false;
		} else {
			string += ",";
		}

		if (!compact)
			string += '\n' + lineTab;

		writeValue(string, value, compact, level);

	}
	
	if (!compact) {
		string += '\n';
		string += lastTab;
	}

	string += ']';

}

void JsonDocument::writeObject(StringType& string, const JsonObject& object, bool compact, u32 level) {

	StringType lineTab(level * IndentationLevel, ' ');
	StringType lastTab = "";

	if (!compact && !object.empty())
		lastTab = StringType((level - 1) * IndentationLevel, IndentationChar);

	string += "{";

	bool first = true;

	for (const auto& [name, value] : object) {

		if (first) {
			first = false;
		} else {
			string += ",";
		}

		if (!compact)
			string += '\n' + lineTab;

		string += '"' + name + "\":";

		if (!compact)
			string += ' ';

		writeValue(string, value, compact, level);

	}

	if (!compact) {
		string += '\n';
		string += lastTab;
	}

	string += '}';

}
