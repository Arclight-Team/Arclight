/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 argumentparser.hpp
 */

#pragma once

#include "common/concepts.hpp"
#include "common/exception.hpp"
#include "util/string.hpp"
#include "types.hpp"

#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <variant>
#include <algorithm>


class ArgumentParserException : public ArclightException {

public:

	using ArclightException::ArclightException;

	template<class... Args>
	explicit ArgumentParserException(const std::string& msg, Args&&... args) : ArclightException(String::format(msg, std::forward<Args>(args)...)) {}

	virtual const char* name() const noexcept override { return "Argument Parser Exception"; }

};


class ArgumentLayoutException : public ArclightException {

public:

	explicit ArgumentLayoutException(const std::string& cause, const std::string& layout, SizeT pos) noexcept : ArclightException("Syntax error: " + cause), layout(layout), position(pos) {}

	virtual const char* name() const noexcept override { return "Argument Layout Exception"; }

	constexpr const std::string& getLayout() const noexcept {
		return layout;
	}

	constexpr SizeT getErrorPosition() const noexcept {
		return position;
	}

private:

	std::string layout;
	SizeT position;

};


namespace CC {

	template<class T>
	concept ArgumentType = Integer<T> || Equal<T, std::string>;

}


class ArgumentParser {

private:

	enum class TokenType {
		SectionOpener,		// < [
		SectionCloser,		// > ]
		OperatorAnd,		// |
		OperatorOr,			// ,
		Separator,			// : =
		String,
		End
	};

	struct Token {

		constexpr Token() noexcept : Token("", TokenType::End) {}
		constexpr Token(const std::string& str, TokenType type) : str(str), type(type) {}

		std::string str;
		TokenType type;

	};

	struct Argument {

		enum class Type {
			Flag,
			Int,
			UInt,
			Word,
			String,
		};

		using enum Type;

		std::vector<std::string> names;
		Type type = Type::Flag;
		char separator = ' ';

	};

	enum class Operator {
		None,
		Or,
		And
	};

	struct Section {

		enum class Type {
			None,
			Argument,
			Container
		};

		using enum Type;

		bool optional;
		Operator op;

		bool dirty = false;
		Type type = Type::None;

	};

	// Layout parser state
	enum class LayoutState {
		Section,
		ArgumentName,
		ArgumentSeparator,
		ArgumentType
	};

	// Argument parser state
	enum class State {
		Match,		// Argument matching
		MatchOpt,	// Optional argument matching due not not being passed
		Error		// Argument not matching
	};


	using ValueT = std::variant<std::string, u32, i32>;

public:

	inline ArgumentParser() noexcept : argCursor(1), cursor(0), tokenStart(0) {}

	inline ArgumentParser(const std::vector<std::string>& args, const std::string& layout) : argCursor(1), cursor(0), tokenStart(0), arguments(args), layout(layout) {}

	inline ArgumentParser(int argc, char* argv[], const std::string& layout) : argCursor(1), cursor(0), tokenStart(0), layout(layout) {
		setArguments(argc, argv);
	}


	inline void parse() {
		parseArguments();
	}

	inline void parse(const std::vector<std::string>& args, const std::string& layout) {
		this->layout = layout;
		arguments = args;
		parseArguments();
	}

	inline void parse(int argc, char* argv[], const std::string& layout) {
		this->layout = layout;
		setArguments(argc, argv);
		parseArguments();
	}


	inline bool contains(const std::string& name) const {
		return aliases.contains(name);
	}


	inline i32 getInt(const std::string& name) const {
		return getValue<i32>(name);
	}

	inline u32 getUInt(const std::string& name) const {
		return getValue<u32>(name);
	}

	inline std::string getString(const std::string& name) const {
		return getValue<std::string>(name);
	}

	inline i32 getInt(const std::string& name, i32 defaultValue) const {
		return getValue<i32>(name, defaultValue);
	}

	inline u32 getUInt(const std::string& name, u32 defaultValue) const {
		return getValue<u32>(name, defaultValue);
	}

	inline std::string getString(const std::string& name, const std::string& defaultValue) const {
		return getValue<std::string>(name, defaultValue);
	}


	inline bool getFlag(const std::string& name) const {
		return flags.contains(name);
	}


	constexpr const std::vector<std::string>& getArguments() const {
		return arguments;
	}

	constexpr const std::string& getLayout() const {
		return layout;
	}

private:

	constexpr void setArguments(int argc, char* argv[]) {

		try {

			arguments.reserve(argc);

			for (SizeT i = 0; i < argc; i++) {
				arguments.emplace_back(argv[i]);
			}

		} catch (...) {

			throw ArgumentParserException("Could not store given arguments");

		}

	}


	template<class T>
	T getValueUnsafe(const std::string& name) const {

		ValueT value = values.at(aliases.at(name));

		if (!std::holds_alternative<T>(value)) {
			throw ArgumentParserException("Argument \"" + name + "\" does not hold given type");
		}

		return std::get<T>(value);

	}

	template<class T>
	T getValue(const std::string& name, const T& defaultValue) const {

		if (!aliases.contains(name)) {
			return defaultValue;
		}

		return getValueUnsafe<T>(name);

	}

	template<class T>
	T getValue(const std::string& name) const {

		if (!aliases.contains(name)) {
			throw ArgumentParserException("Argument \"" + name + "\" does not exist");
		}

		return getValueUnsafe<T>(name);

	}


	void parseArguments() {

		std::vector<Section> sections; // Current sections
		Argument arg; // Temporary argument data

		LayoutState layoutState = LayoutState::Section; // Layout parsing state (unrelated to the states variable)

		std::vector<State> states = { State::Match }; // Current expression states for each depth (container sections and root)

		Operator lastOperator = Operator::None;
		bool leftOperand = false;

		cursor = 0;
		tokenStart = 0;

		Token token;

		while (token = tokenize(), token.type != TokenType::End) {

			switch (token.type) {
				case TokenType::SectionOpener: { // Open a section (argument or container)

					if (leftOperand) {
						throw ArgumentLayoutException("Unexpected section opener, Operator expected", layout, cursor);
					}

					if (!sections.empty()) {

						if (sections.back().type == Section::Argument) {
							throw ArgumentLayoutException("Attempted to open a section inside an argument section", layout, cursor);
						}

						if (sections.back().type != Section::Container) { // If opening section inside another
							sections.back().type = Section::Container;
							states.push_back(State::Match);
						}

					}

					sections.push_back(Section{token.str == "[", lastOperator});

					lastOperator = Operator::None;

					break;

				} case TokenType::SectionCloser: { // Close a section (argument or container), the argument parsing logic is handled here

					if (sections.empty()) {
						throw ArgumentLayoutException("Attempted to close non existing section", layout, cursor);
					} else if (sections.back().optional != (token.str == "]")) {
						throw ArgumentLayoutException("Attempted to close section of the wrong type", layout, cursor);
					} else if (layoutState == LayoutState::ArgumentType) {
						throw ArgumentLayoutException("Unexpected section closer, argument type string expected", layout, cursor);
					} else if (layoutState == LayoutState::ArgumentName) {
						throw ArgumentLayoutException("Unexpected section closer, argument name string expected", layout, cursor);
					} else if (sections.back().type == Section::None) {
						throw ArgumentLayoutException("Illegal empty section", layout, cursor);
					}

					const SizeT depth = sections.size();
					Section last = sections.back();

					layoutState = LayoutState::Section;

					// Helper lambda to parse the next argument and set the right state if successful or not
					auto doParse = [&](bool optional) {

						if (parseArgument(arg)) { // If argument matches

							states[depth - 1] = State::Match;

							// Make previous sections dirty
							for (SizeT i = 0; i < depth - 1; i++) {
								sections[i].dirty = true;
							}

						} else {

							states[depth - 1] = optional ? State::MatchOpt : State::Error;

						}

					};

					if (last.type == Section::Argument) {

						// states[depth - 1] is the state in the argument's depth

						if (states[depth - 1] == State::Error) { // Error state

							// Check the branch for container sections
							if (last.op != Operator::Or && (depth == 1 || sections[depth - 2].dirty)) {
								throw ArgumentParserException("Arguments not matching layout");
							}

							doParse(last.optional);

						} else if (states[depth - 1] == State::MatchOpt) { // Optional matching state

							if (last.op == Operator::Or) {
								doParse(true);
							} else {
								doParse(last.optional);
							}

						} else if (last.op != Operator::Or) { // Matching state (ignored if the operator is Or)

							doParse(last.optional);

						}

						// Reset temporary argument
						arg = Argument{};

					} else if (last.type == Section::Container) {

						// states[depth] is the state in this current section
						// states[depth - 1] is the state in the previous depth
						// Note: the expression root is not a section, so, states[0] is used for it

						// If error, non-optional and dirty (unrecoverable state), throw
						if (states[depth] == State::Error && !last.optional && last.dirty) {
							throw ArgumentParserException("Arguments not matching layout");
						}

						// If optional and not dirty (state does not matter), update the state to optional matching
						if (last.optional && !last.dirty) {
							states[depth] = State::MatchOpt;
						}

						if (states[depth - 1] == State::Error) { // Previous depth error

							// Previous expression error causes a throw only if in root or if dirty
							// Else, leave the error state because future expressions could match instead
							if (last.op != Operator::Or && (depth == 1 || last.dirty)) {
								throw ArgumentParserException("Arguments not matching layout");
							}

							states[depth - 1] = states[depth];

						} else if (states[depth - 1] == State::Match) { // Previous depth matching

							if (last.op != Operator::Or) {
								states[depth - 1] = states[depth];
							} else if (last.dirty) {
								throw ArgumentParserException("Arguments not matching layout");
							}

						} else { // Previous depth optional matching

							if (last.op != Operator::Or || states[depth] != State::Error) {
								states[depth - 1] = states[depth];
							}

						}

						states.pop_back();

					}

					sections.pop_back();

					leftOperand = true;

					break;

				} case TokenType::OperatorAnd: { // And operator for sections

					if (layoutState == LayoutState::Section && leftOperand) {

						leftOperand = false;
						lastOperator = Operator::And;

					} else {

						throw ArgumentLayoutException("Unexpected And operator", layout, cursor);

					}

					break;

				} case TokenType::OperatorOr: { // Or operator for sections and argument names

					if (layoutState == LayoutState::Section && leftOperand) {

						leftOperand = false;
						lastOperator = Operator::Or;

					} else if (layoutState == LayoutState::ArgumentSeparator) {

						layoutState = LayoutState::ArgumentName;

					} else {

						throw ArgumentLayoutException("Unexpected Or operator", layout, cursor);

					}

					break;

				} case TokenType::Separator: { // Argument separator character (' ' by default)

					if (layoutState != LayoutState::ArgumentSeparator) {
						throw ArgumentLayoutException("Unexpected separator character", layout, cursor);
					}

					arg.separator = token.str[0];

					layoutState = LayoutState::ArgumentType;

					break;

				} case TokenType::String: { // Argument name or type

					if (sections.empty()) {
						throw ArgumentLayoutException("Unexpected string", layout, cursor);
					} else if (sections.back().type == Section::Container) {
						throw ArgumentLayoutException("Illegal string inside container section", layout, cursor);
					}

					if (sections.back().type == Section::None) {
						layoutState = LayoutState::ArgumentName;
					} else if (layoutState == LayoutState::Section) {
						throw ArgumentLayoutException("Illegal string after argument end", layout, cursor);
					}

					std::string tokenStr(token.str);

					if (layoutState == LayoutState::ArgumentName) {

						sections.back().type = Section::Argument;
						arg.names.push_back(tokenStr);

						layoutState = LayoutState::ArgumentSeparator;

					} else {

						static std::unordered_map<std::string, Argument::Type> map = {
							{"int",		Argument::Int},
							{"uint",	Argument::UInt},
							{"word",	Argument::Word},
							{"string",	Argument::String}
						};

						if (!map.contains(tokenStr)) {
							throw ArgumentLayoutException("String \"" + tokenStr + "\" does not represent a type", layout, cursor);
						}

						arg.type = map[tokenStr];

						layoutState = LayoutState::Section;

					}

					break;

				}
			}

		}

		if (lastOperator != Operator::None) {
			throw ArgumentLayoutException("Layout ends with operator", layout, cursor);
		} else if (!sections.empty()) {
			throw ArgumentLayoutException("Layout ends with " + std::to_string(sections.size()) + " opened sections", layout, cursor);
		} else if (states[0] == State::Error || argCursor < arguments.size()) {
			throw ArgumentParserException("Arguments do not match layout");
		}

	}

	bool parseArgument(const Argument& arg) {

		SizeT cur = argCursor;

		if (cur + 1 > arguments.size()) {
			return false;
		}

		std::string name;
		std::string valueStr;

		// Get argument name and value string

		if (arg.separator != ' ') {

			std::string str = arguments[cur++];

			SizeT end = str.find_first_of(arg.separator);

			if (end == std::string::npos) {
				return false;
			}

			name = str.substr(0, end);
			valueStr = str.substr(end + 1, str.length() - end);

		} else {

			name = arguments[cur++];

			if (arg.type != Argument::Flag) {

				if (cur + 1 > arguments.size()) {
					return false;
				}

				valueStr = arguments[cur++];

			}

		}

		// Validate argument name

		if (std::find(arg.names.begin(), arg.names.end(), name) == arg.names.end()) {
			return false;
		}

		// Store argument

		if (arg.type == Argument::Flag) {

			for (const auto& n : arg.names) {

				if (!flags.insert(n).second) {
					throw ArgumentParserException("Flag \"" + n + "\" already inserted");
				}

			}

			argCursor = cur;

			return true;

		}

		for (const auto& n : arg.names) { // Store aliases for argument getters
			if (!aliases.try_emplace(n, arg.names[0]).second) {
				throw ArgumentParserException("Argument \"" + n + "\" already inserted");
			}
		}

		switch (arg.type) {
			case Argument::Word:

				if (valueStr.find(' ') != std::string::npos) {
					throw ArgumentParserException("Value of argument \"" + name + "\" is not a word");
				}

			case Argument::String:

				values.try_emplace(arg.names[0], valueStr);
				break;

			case Argument::UInt:
			case Argument::Int:

				i64 number;

				std::istringstream is(valueStr);
				is.setf(std::ios_base::hex | std::ios_base::dec);
				is >> number;

				if (is.fail() || !is.eof()) {
					throw ArgumentParserException("Value of argument \"" + name + "\" is not an integer");
				}

				u32 unumber = number;
				i32 inumber = number;

				if (arg.type == Argument::UInt && unumber == number) {
					values.try_emplace(arg.names[0], unumber);
				} else if (arg.type == Argument::Int && inumber == number) {
					values.try_emplace(arg.names[0], inumber);
				} else {
					throw ArgumentParserException("Integer value of argument \"" + name + "\" out of range");
				}

		}

		argCursor = cur;

		return true;

	}


	Token tokenize() {

		tokenStart = cursor;
		TokenType type = scanNextToken();

		return { std::string(layout.begin() + tokenStart, layout.begin() + cursor), type };

	}

	TokenType scanNextToken() {

		TokenType type = TokenType::End;

		while (cursor < layout.length()) {

			char c = layout[cursor];
			cursor++;

			switch (c) {
				case '<':
				case '[':
					return TokenType::SectionOpener;
				case '>':
				case ']':
					return TokenType::SectionCloser;
				case ',':
					return TokenType::OperatorAnd;
				case '|':
					return TokenType::OperatorOr;
				case ':':
				case '=':
					return TokenType::Separator;
				case '\n':
				case '\t':
				case ' ':
					tokenStart++;
					break;
				default:

					SizeT end = layout.find_first_of("<[>],|:=\n\t ", cursor);

					if (end == std::string::npos) {
						throw ArgumentLayoutException("Layout ends with unfinished string", layout, cursor);
					}

					cursor = end;

					return TokenType::String;
			}

		}

		return type;

	}


	SizeT cursor;
	SizeT tokenStart;
	SizeT argCursor;

	std::string layout;
	std::vector<std::string> arguments;

	std::unordered_set<std::string> flags;
	std::unordered_map<std::string, ValueT> values;
	std::unordered_map<std::string, std::string> aliases;

};
