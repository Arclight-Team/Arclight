/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 document.hpp
 */

#pragma once

#include "common.hpp"
#include "object.hpp"
#include "array.hpp"
#include "util/char.hpp"
#include <sstream>



class JsonDocument {

public:

	using StringType = Json::StringType;
	using StringView = Json::StringView;
	using StringIterator = StringView::iterator;
	using StringConstIterator = StringView::const_iterator;

	static constexpr u32 IndentationLevel = 2;
	static constexpr char IndentationChar = ' ';

	JsonDocument() = default;
	JsonDocument(const JsonObject& root);
	JsonDocument(const StringView& json);

	void read(const StringView& json);
	StringType write(bool compact = false) const;

	void clear();
	bool empty() const;

	void setRoot(const JsonObject& root);
	const JsonObject& getRoot();

	static JsonDocument fromFile(const Path& path);

private:

	struct Iterator {

		StringConstIterator cur;
		StringConstIterator end;

		constexpr bool matchSequence(const StringView& seq) const {

			if (seq.length() > rem())
				return false;

			for (u32 i = 0; i < seq.length(); i++) {

				if (cur[i] != seq[i])
					return false;

			}

			return true;

		}

		constexpr bool matchAny(const StringView& seq) const {

			for (u32 i = 0; i < seq.length(); i++) {

				if (cur[0] == seq[i])
					return true;

			}

			return false;

		}

		constexpr i64 rem() const {
			return end - cur;
		}

		constexpr bool overflown() const {
			return cur > end;
		}

	};

	static constexpr u32 ReadStepName = 0;
	static constexpr u32 ReadStepColon = 1;
	static constexpr u32 ReadStepValue = 2;
	static constexpr u32 ReadStepComma = 3;

	bool readComment(Iterator& it);
	bool readString(Iterator& it, StringType& string);
	bool readNumber(Iterator& it, JsonValue& value);
	void readValue(Iterator& it, JsonValue& value);
	bool readName(Iterator& it, StringType& name, bool& closed);
	bool readComma(Iterator& it, char closingChar, bool& closed);
	bool readColon(Iterator& it);
	void readArray(Iterator& it, JsonArray& array);
	void readObject(Iterator& it, JsonObject& object);

	void writeValue(StringType& string, const JsonValue& value, bool compact, u32 level) const;
	void writeArray(StringType& string, const JsonArray& array, bool compact, u32 level) const;
	void writeObject(StringType& string, const JsonObject& object, bool compact, u32 level) const;

	JsonObject root;

};

RawLog& operator<<(RawLog& log, const JsonDocument& document);