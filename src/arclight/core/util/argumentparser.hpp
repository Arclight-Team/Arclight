/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 argumentparser.hpp
 */

#pragma once

#include "common/concepts.hpp"
#include "common/exception.hpp"
#include "util/bool.hpp"
#include "util/string.hpp"
#include "util/assert.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <variant>
#include <algorithm>


class ArgumentParserException : public ArclightException {

public:

	using ArclightException::ArclightException;

	template<class... Args>
	explicit ArgumentParserException(const std::string& msg, Args&&... args) : ArclightException(String::format(msg, std::forward<Args>(args)...)) {}

	const char* name() const noexcept override { return "Argument Parser Exception"; }

};


class ArgumentLayoutException : public ArclightException {

public:

	explicit ArgumentLayoutException(const std::string& reason, std::string layout, SizeT pos) noexcept : ArclightException("Syntax error: " + reason), layout(std::move(layout)), position(pos) {}

	const char* name() const noexcept override { return "Argument Layout Exception"; }

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


class ArgumentLayout {

public:

	struct Argument {

		enum class Type {
			Flag,
			Int,
			UInt,
			Float,
			Double,
			String,
			Word,
		};

		using enum Type;


		constexpr explicit Argument(Type type = Flag, bool unnamed = false, char separator = ' ')
				: type(type), unnamed(unnamed), separator(separator) {}


		// Returns the amount of command line arguments required to represent this argument
		constexpr SizeT stringsCount() const noexcept {
			return 1 + (type != Type::Flag && !unnamed && separator == ' ');
		}


		Type type;
		bool unnamed;
		char separator;
		std::vector<std::string> names;

	};

	struct Node {

		using ChildrenT = std::vector<Node>;

		enum class Type {
			Unknown,
			Mandatory,
			Optional,
			Unordered
		};

		enum class Operator {
			None,
			And,
			Or
		};

		using enum Type;
		using enum Operator;


		constexpr explicit Node(Type type, Operator op = None) : type(type), op(op) {}


		constexpr bool isArgument() const noexcept {
			return children.empty();
		}

		constexpr bool isParent() const noexcept {
			return !isArgument();
		}

		constexpr bool isOptional() const noexcept {
			return type == Optional;
		}

		constexpr bool isUnordered() const noexcept {
			return type == Unordered;
		}


		Type type;
		Operator op;

		Argument argument;

		ChildrenT children;

	};


	constexpr ArgumentLayout() : rootNode(Node::Mandatory) {}

	constexpr ArgumentLayout(const std::string& layout) : ArgumentLayout() {
		buildTree(layout);
	}

	constexpr ArgumentLayout(const char* layout) : ArgumentLayout() {
		buildTree(layout);
	}


	constexpr const Node& getRootNode() const {
		return rootNode;
	}

private:

	class TreeBuilder {

	public:

		constexpr explicit TreeBuilder(Node& root) {
			levels.emplace_back(&root);
		}


		constexpr Node& current() {
			return *levels.back();
		}

		constexpr const Node& current() const {
			return *levels.back();
		}


		constexpr SizeT size() const noexcept {
			return levels.size() - 1; // Root node does not count in the size
		}

		constexpr bool empty() const noexcept {
			return !size();
		}


		constexpr void push(const Node& node) {

			auto& parent = current();

			auto& child = parent.children.emplace_back(node);

			levels.emplace_back(&child);

		}


		constexpr bool pop() {

			if (empty()) { // Root node cannot be popped
				return false;
			}

			levels.pop_back();

			return true;

		}

	private:

		std::vector<Node*> levels;

	};

	enum class State {
		PreChild,	// Force child begin
		ChildBegin,	// Accepts child begin, unnamed modifier and argument name
		ChildEnd,	// Accepts operator
		PreName,	// Force argument name
		Name,		// Accepts separator, or operator (for multiple names) and type
		PreType,	// Force argument type
		Type		// Force child end
	};


	constexpr void buildTree(const std::string& layout) {

		State state = State::PreChild;

		SizeT tokenStart = 0;
		SizeT tokenCursor = 0;

		TreeBuilder tree(rootNode);

		Node::Operator lastOperator = Node::None;


		auto throwException = [&](const std::string& message) {
			throw ArgumentLayoutException(message, layout, tokenStart);
		};

		auto pushNode = [&](Node::Type type) {

			if (Bool::none(state, State::PreChild, State::ChildBegin)) {
				throwException("Unexpected parent begin token");
			}

			state = State::ChildBegin;

			tree.push(Node(type, lastOperator));

			lastOperator = Node::None;

		};

		auto popNode = [&](Node::Type type) {

			const auto& node = tree.current();

			if (!tree.empty() && node.type != type) {
				throwException("Section end token does not match section type");
			} else if (Bool::none(state, State::Type, State::Name, State::ChildEnd)) {
				throwException("Unexpected section end token");
			} else if (node.argument.unnamed && node.argument.type == Argument::Flag) {
				throwException("Flag argument cannot be marked as unnamed");
			} else if (node.isUnordered() && node.isArgument()) {
				throwException("Argument node cannot be unordered");
			}

			state = State::ChildEnd;

			if (!tree.pop()) {
				throwException("Exceeding section end token");
			} else if (tree.current().isUnordered() && node.isParent()) {
				throwException("Unordered parent can only contain argument nodes");
			}

		};

		auto setOperator = [&](Node::Operator type) {

			bool nameOperator = (type == Node::Or) && (state == State::Name);

			if (!nameOperator && state != State::ChildEnd) {
				throwException("Unexpected operator token");
			} if (type != Node::And && tree.current().isUnordered()) {
			throwException("Unordered parent can only contain And operators");
		}

			state = nameOperator ? State::PreName : State::PreChild;

			if (!nameOperator) {
				lastOperator = type;
			}

		};

		auto setSeparator = [&](char c) {

			auto& argument = tree.current().argument;

			if (state != State::Name) {
				throwException("Unexpected separator token");
			} else if (argument.unnamed) {
				throwException("Unnamed argument cannot have a separator");
			}

			state = State::PreType;

			argument.separator = c;

		};

		auto setUnnamed = [&]() {

			if (state != State::ChildBegin) {
				throwException("Unexpected unnamed modifier token");
			}

			state = State::PreName;

			tree.current().argument.unnamed = true;

		};

		auto setString = [&]() {

			auto& argument = tree.current().argument;

			tokenCursor = layout.find_first_of("<>[]{}|,&:=#\n\t ", tokenCursor);

			bool name = Bool::any(state, State::ChildBegin, State::PreName);
			bool type = Bool::any(state, State::PreType, State::Name);

			if (tokenCursor == std::string::npos) {
				throwException("Unexpected string token at the end of layout");
			} else if (!name && !type) {
				throwException("Unexpected string token");
			}

			std::string token = layout.substr(tokenStart, tokenCursor - tokenStart);

			if (name) {

				argument.names.emplace_back(token);

				state = State::Name;

				return;

			}

			state = State::Type;

			static const std::unordered_map<std::string_view, Argument::Type> argumentTypes = {
					{"int",		Argument::Int},
					{"uint",	Argument::UInt},
					{"float",	Argument::Float},
					{"double",	Argument::Double},
					{"string",	Argument::String},
					{"word",	Argument::Word}
			};

			if (!argumentTypes.contains(token)) {
				throwException("String does not represent an argument type");
			}

			argument.type = argumentTypes.at(token);

		};


		while (tokenCursor < layout.length()) {

			tokenStart = tokenCursor;

			char c = layout[tokenCursor++];

			switch (c) {

				case '<':	pushNode(Node::Mandatory);		break;
				case '[':	pushNode(Node::Optional);		break;
				case '{':	pushNode(Node::Unordered);	break;
				case '>':	popNode(Node::Mandatory);		break;
				case ']':	popNode(Node::Optional);		break;
				case '}':	popNode(Node::Unordered);	break;

				case ',':
				case '&':	setOperator(Node::And);	break;
				case '|':	setOperator(Node::Or);	break;

				case ':':
				case '=':	setSeparator(c);	break;

				case '#':	setUnnamed();	break;

				case '\n':
				case '\t':
				case ' ':	continue;

				default:	setString(); break;

			}

		}


		if (!tree.empty()) {
			throwException("Layout ends with unfinished node");
		}

	}


	Node rootNode;

};


namespace CC {

	template<class T>
	concept ArgumentType = CC::Equal<T, std::string> || CC::Integer<T> || CC::Float<T>;

}


class ArgumentParser {

private:

	using Node = ArgumentLayout::Node;
	using Argument = ArgumentLayout::Argument;

public:

	using ValueT = std::variant<std::string, u64, i64, float, double>;


	ArgumentParser();

	explicit ArgumentParser(const std::vector<std::string>& args, ArgumentLayout layout);

	explicit ArgumentParser(int argc, char* argv[], ArgumentLayout layout);


	void parse();

	void parse(const std::vector<std::string>& args, const ArgumentLayout& layout);

	void parse(int argc, char* argv[], const ArgumentLayout& layout);

	void reset();


	void setDescription(const std::string& name, u32 index, const std::string& description);

	void setDescription(const std::string& name, const std::string& description);

	const std::string& getDescription(const std::string& name, u32 index) const;

	const std::string& getDescription(const std::string& name) const;

	bool hasDescription(const std::string& name, u32 index) const;

	bool hasDescription(const std::string& name) const;


	bool containsValue(const std::string& name) const;

	bool containsFlag(const std::string& name) const;

	bool contains(const std::string& name) const;


	template<CC::ArgumentType T>
	inline T get(const std::string& name) const {

		try {
			return convertedValue<T>(name);
		} catch (...) {
			throw ArgumentParserException("Argument \"" + name + "\" does not exist");
		}

	}

	template<CC::ArgumentType T>
	inline T get(const std::string& name, const T& defaultValue) const {

		try {
			return convertedValue<T>(name);
		} catch (...) {
			return defaultValue;
		}

	}


	i64 getInt(const std::string& name) const;

	u64 getUInt(const std::string& name) const;

	std::string getString(const std::string& name) const;

	float getFloat(const std::string& name) const;

	double getDouble(const std::string& name) const;


	i64 getInt(const std::string& name, i64 defaultValue) const;

	u64 getUInt(const std::string& name, u64 defaultValue) const;

	std::string getString(const std::string& name, const std::string& defaultValue) const;

	float getFloat(const std::string& name, float defaultValue) const;

	double getDouble(const std::string& name, double defaultValue) const;


	bool getFlag(const std::string& name) const;

	bool getFlag(const std::string& name, bool defaultValue) const;


	constexpr const ArgumentLayout& getLayout() const {
		return layout;
	}

	constexpr const std::vector<std::string>& getArguments() const {
		return arguments;
	}

private:

	struct ParseState {

		using AliasT = std::pair<std::string, std::string>;
		using ArgumentT = std::pair<std::string, std::optional<ValueT>>;


		constexpr explicit ParseState(SizeT cursor) noexcept : cursor(cursor) {}


		constexpr void inherit(const ParseState& other) {

			// Inherit state from another instance

			cursor = other.cursor;

			arguments.insert(arguments.end(), other.arguments.begin(), other.arguments.end());
			aliases.insert(aliases.end(), other.aliases.begin(), other.aliases.end());

		}


		std::vector<ArgumentT> arguments; // If the optional has no value, it is a flag
		std::vector<AliasT> aliases;

		SizeT cursor;

	};

	struct ParseLevel {

		enum class Match {
			Full,		// Argument matching
			Partial,	// Optional argument not matching
			None		// Argument not matching
		};

		using enum Match;


		explicit ParseLevel(Match match, SizeT cursor) noexcept : state(cursor), match(match), dirty(false), fatal(false) {}


		ParseState state; // Current parsing state (cursor and parsed arguments)

		Match match; // Current match

		bool dirty;
		bool fatal; // Unrecoverable state, becomes an exception in root

	};

	struct ParseNode {

		constexpr explicit ParseNode(const Node& node) noexcept : node(node), expanded(false) {}


		const Node& node;

		bool expanded;

	};

	enum class StackOperation {
		Pop,
		Expand,
		None,
	};

	using DescriptionsT = std::unordered_map<u32, std::string>;

	static constexpr u32 DefaultDescriptionID = -1;


	constexpr void loadArguments(int argc, char* argv[]) {

		try {

			arguments.reserve(argc);

			for (SizeT i = 0; i < argc; i++) {
				arguments.emplace_back(argv[i]);
			}

		} catch (...) {

			throw ArgumentParserException("Could not store given arguments");

		}

	}


	inline ValueT getValue(const std::string& name) const;


	template<class T, CC::Integral I>
	static constexpr bool compatibleIntegral(const ValueT& value) {
		return std::holds_alternative<T>(value) && std::in_range<I>(std::get<T>(value));
	}

	template<CC::ArgumentType T>
	inline T convertedValue(const std::string& name) const {

		ValueT value = getValue(name);

		if constexpr (CC::Equal<T, std::string>) {

			if (std::holds_alternative<T>(value)) {
				return std::get<T>(value);
			}

		} else if constexpr (CC::Float<T>) {

			if (std::holds_alternative<double>(value)) {
				return std::get<double>(value);
			} else if (std::holds_alternative<float>(value)) {
				return std::get<float>(value);
			}

		} else if constexpr (CC::Integer<T>) {

			if (compatibleIntegral<i64, T>(value)) {
				return std::get<i64>(value);
			} else if (compatibleIntegral<u64, T>(value)) {
				return std::get<u64>(value);
			}

		}

		throw ArgumentParserException("Argument \"" + name + "\" type is incompatible with given type");

	}


	template<class T>
	inline T typedValue(const std::string& name) const {

		ValueT value = getValue(name);

		if (!std::holds_alternative<T>(value)) {
			throw ArgumentParserException("Argument \"" + name + "\" does not hold expected type");
		}

		return std::get<T>(value);

	}

	template<class T>
	inline T typedValue(const std::string& name, const T& defaultValue) const {

		try {
			return typedValue<T>(name);
		} catch (...) {
			return defaultValue;
		}

	}



	static std::optional<ValueT> parseValue(const std::string& string, Argument::Type type) {

		std::optional<ValueT> value;

		switch (type) {

			case Argument::Int:		value = parseArithmetic<i64>(string);		break;
			case Argument::UInt:	value = parseArithmetic<u64>(string);		break;
			case Argument::Float:	value = parseArithmetic<float>(string);		break;
			case Argument::Double:	value = parseArithmetic<double>(string);	break;

			case Argument::Word:

				if (string.find(' ') != std::string::npos) {
					return {};
				}

				[[fallthrough]];

			case Argument::String:

				value = string;

				break;

			default:

				arc_force_assert("Illegal argument type");

		}

		return value;

	}

	template<CC::Arithmetic A>
	static std::optional<ValueT> parseArithmetic(const std::string& string) {

		A value;

		const char* end = string.data() + string.size();

		const auto& result = std::from_chars(string.data(), end, value);

		if (result.ec != std::errc{} || result.ptr != end) {
			return {};
		}

		return value;

	}


	bool parseArgument(const Argument& argument, ParseState& storage);


	void evaluateNode(const Node& node, ParseLevel::Match& state, std::vector<ParseLevel>& levels, bool acceptOptional = true);

	void evaluateUnorderedParent(const Node& node, ParseLevel::Match& state, std::vector<ParseLevel>& levels);


	StackOperation processParent(const Node& node, std::vector<ParseLevel>& levels);

	bool processExpandedParent(const Node& node, std::vector<ParseLevel>& levels);

	void processNode(const Node& node, std::vector<ParseLevel>& levels, ParseLevel::Match& state);


	void parseTree();


	ArgumentLayout layout;

	std::vector<std::string> arguments;

	std::unordered_set<std::string> flags;
	std::unordered_map<std::string, ValueT> values;
	std::unordered_map<std::string, std::string> aliases;
	std::unordered_map<std::string, DescriptionsT> descriptions;

};
