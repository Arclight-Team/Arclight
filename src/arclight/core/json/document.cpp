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
#include "util/log.hpp"



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

auto JsonDocument::write(bool compact) const -> StringType {

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

JsonObject& JsonDocument::getRoot() {
	return root;
}

const JsonObject& JsonDocument::getRoot() const {
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

void JsonDocument::readString(Iterator& it, StringType& string, bool name) {

	if (it.cur[0] != '"')
		throw JsonSyntaxError("Missing string opener");

	bool stop = false;
	bool escape = false;
	bool terminated = false;

	for (it.cur++; it.cur < it.end; it.cur++) {

		auto ch = it.cur[0];

		if (escape) {

			switch (ch) {

			case '\"':
			case '\\':
			case '/':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
				escape = false;
				break;

				// Invalid escape sequence
			default:
				throw JsonSyntaxError("Invalid escape sequence found");
				return;

			}

		} else {

			switch (ch) {

				// Names cannot contain tabs
			case '\t':
				if (!name)
					break;
				
				throw JsonSyntaxError("Tab found inside name");

				// String terminator
			case '\"':
				terminated = true;
				[[fallthrough]];

				// Strings cannot contain newlines
			case '\n':
			case '\r':
				stop = true;
				break;

				// Escape sequence start
			case '\\':
				escape = true;
				break;

			}

		}

		if (stop) {

			// Unterminated string
			if (!terminated) {
				throw JsonSyntaxError("Unterminated string sequence found");
			}

			return;
		}

		string += it.cur[0];

	}

	throw JsonSyntaxError("Unexpected EOF while reading string");

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

bool JsonDocument::readValue(Iterator& it, JsonValue& value, bool array) {

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

		case ']': // Empty array

			if (!array)
				throw JsonSyntaxError("Invalid symbol found in value definition");

			return true;

		case '"': // String
		{
			StringType string;

			readString(it, string, false);
		
			value = string;
			return false;
		}

		case '{': // Object
		{
			JsonObject object;

			readObject(it, object);

			value = object;
			return false;
		}

		case '[': // Array
		{
			JsonArray array;

			readArray(it, array);

			value = array;
			return false;
		}

		case 't': // true
		{
			if (!it.matchSequence("true"))
				throw JsonSyntaxError("Invalid symbol found in value definition");

			it.cur += 3;
			value = true;
			return false;
		}

		case 'f': // false
		{
			if (!it.matchSequence("false"))
				throw JsonSyntaxError("Invalid symbol found in value definition");

			it.cur += 4;
			value = false;
			return false;
		}

		case 'n': // null
		{
			if (!it.matchSequence("null"))
				throw JsonSyntaxError("Invalid symbol found in value definition");

			it.cur += 3;
			value = nullptr;
			return false;
		}

		default: // Number

			if (ch == '-' || Character::isDigit(ch)) {

				if (readNumber(it, value))
					return false;

			}

			throw JsonSyntaxError("Invalid symbol found in value definition");

		}

	}

	throw JsonSyntaxError("Unexpected EOF while reading value");

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
			readString(it, name, true);
			return true;

		}

	}

	throw JsonSyntaxError("Unexpected EOF while reading name");

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

	throw JsonSyntaxError("Unexpected EOF while reading comma");

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

	throw JsonSyntaxError("Unexpected EOF while reading colon");

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

				bool closed = readValue(it, value, true);
				if (closed) {

					// Unexpected comma after last element
					if (!array.empty()) {
						throw JsonSyntaxError("Unexpected comma separator after last array element");
					}

					return;
				}

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

	throw JsonSyntaxError("Unexpected EOF while reading array");

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

				if (closed) {

					// Unexpected comma after last element
					if (!object.empty()) {
						throw JsonSyntaxError("Unexpected comma separator after last object element");
					}

					return;
				}

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

				readValue(it, value, false);
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

	throw JsonSyntaxError("Unexpected EOF while reading object");

}



void JsonDocument::writeString(StringType& string, const StringType& value) const {

	string += '"';

	for (auto ch : value) {

		switch (ch) {

		case '\"': string += "\\\""; break;
		case '\\': string += "\\\\"; break;
		case '\b': string += "\b"; break;
		case '\f': string += "\f"; break;
		case '\n': string += "\n"; break;
		case '\r': string += "\r"; break;
		case '\t': string += "\t"; break;

		default:
			string += ch;
			break;

		}

	}

	string += '"';

}

void JsonDocument::writeValue(StringType& string, const JsonValue& value, bool compact, u32 level) const {

	switch (value.getType()) {

		case Json::Type::String:
			writeString(string, value.toString());
			break;

		case Json::Type::Number:

			if (value.isInteger()) {

				auto n = value.toNumber<Json::IntegerType>();
				string += std::to_string(n);

			} else {

				auto f = value.toNumber<Json::FloatType>();
				string += std::to_string(f);

			}

			break;

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
		case Json::Type::None:
			string += "null";
			break;

	}

}

void JsonDocument::writeArray(StringType& string, const JsonArray& array, bool compact, u32 level) const {

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

void JsonDocument::writeObject(StringType& string, const JsonObject& object, bool compact, u32 level) const {

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

		writeString(string, name);

		string += ':';

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



RawLog& operator<<(RawLog& log, const JsonDocument& document) {

	log << document.write();

	return log;

}