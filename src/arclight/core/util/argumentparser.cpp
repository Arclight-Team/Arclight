/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 argumentparser.cpp
 */

#include "argumentparser.hpp"
#include "util/assert.hpp"

#include <stack>
#include <ranges>


ArgumentParser::ArgumentParser() = default;

ArgumentParser::ArgumentParser(const std::vector<std::string>& args, ArgumentLayout layout) : arguments(args), layout(std::move(layout)) {}

ArgumentParser::ArgumentParser(int argc, char* argv[], ArgumentLayout layout) : layout(std::move(layout)) {
	loadArguments(argc, argv);
}


void ArgumentParser::parse() {

	reset();

	try {

		parseTree();

	} catch (const ArgumentParserException& e) {

		reset();

		throw;

	}

}

void ArgumentParser::parse(const std::vector<std::string>& args, const ArgumentLayout& layout) {

	this->layout = layout;

	arguments = args;

	parse();

}

void ArgumentParser::parse(int argc, char* argv[], const ArgumentLayout& layout) {

	this->layout = layout;

	loadArguments(argc, argv);

	parse();

}

void ArgumentParser::reset() {

	flags.clear();
	values.clear();
	aliases.clear();

}


void ArgumentParser::setDescription(const std::string& name, u32 index, const std::string& description) {
	descriptions[name][index] = description;
}

void ArgumentParser::setDescription(const std::string& name, const std::string& description) {
	setDescription(name, DefaultDescriptionID, description);
}

const std::string& ArgumentParser::getDescription(const std::string& name, u32 index) const {

	try {

		return descriptions.at(name).at(index);

	} catch (...) {

		if (index == DefaultDescriptionID) {
			throw ArgumentParserException("Argument %s has no default description", name.c_str());
		} else {
			throw ArgumentParserException("Argument %s has no description at index %lu", name.c_str(), index);
		}

	}

}

const std::string& ArgumentParser::getDescription(const std::string& name) const {
	return getDescription(name, DefaultDescriptionID);
}

bool ArgumentParser::hasDescription(const std::string& name, u32 index) const {
	return !descriptions.contains(name) ? false : descriptions.at(name).contains(index);
}

bool ArgumentParser::hasDescription(const std::string& name) const {
	return hasDescription(name, DefaultDescriptionID);
}


bool ArgumentParser::containsValue(const std::string& name) const {
	return aliases.contains(name);
}

bool ArgumentParser::containsFlag(const std::string& name) const {
	return flags.contains(name);
}

bool ArgumentParser::contains(const std::string& name) const {
	return containsValue(name) || containsFlag(name);
}


i64 ArgumentParser::getInt(const std::string& name) const {
	return typedValue<i64>(name);
}

u64 ArgumentParser::getUInt(const std::string& name) const {
	return typedValue<u64>(name);
}

std::string ArgumentParser::getString(const std::string& name) const {
	return typedValue<std::string>(name);
}

float ArgumentParser::getFloat(const std::string& name) const {
	return typedValue<float>(name);
}

double ArgumentParser::getDouble(const std::string& name) const {
	return typedValue<double>(name);
}


i64 ArgumentParser::getInt(const std::string& name, i64 defaultValue) const {
	return typedValue<i64>(name, defaultValue);
}

u64 ArgumentParser::getUInt(const std::string& name, u64 defaultValue) const {
	return typedValue<u64>(name, defaultValue);
}

std::string ArgumentParser::getString(const std::string& name, const std::string& defaultValue) const {
	return typedValue<std::string>(name, defaultValue);
}

float ArgumentParser::getFloat(const std::string& name, float defaultValue) const {
	return typedValue<float>(name, defaultValue);
}

double ArgumentParser::getDouble(const std::string& name, double defaultValue) const {
	return typedValue<double>(name, defaultValue);
}


bool ArgumentParser::getFlag(const std::string& name) const {
	return containsFlag(name);
}

bool ArgumentParser::getFlag(const std::string& name, bool defaultValue) const {
	return containsFlag(name) ? true : defaultValue;
}


ArgumentParser::ValueT ArgumentParser::getValue(const std::string& name) const {

	try {
		return values.at(aliases.at(name));
	} catch (...) {
		throw ArgumentParserException("Argument \"" + name + "\" does not exist");
	}

}


bool ArgumentParser::parseArgument(const Argument& argument, ParseState& storage) {

	SizeT cursor = storage.cursor;

	SizeT requirement = argument.stringsCount();

	if (requirement > arguments.size() - cursor) {
		return false;
	}


	std::string string = arguments[cursor++];

	std::string nameString;
	std::string valueString;

	if (argument.separator != ' ') {

		// Split argument string with the separator character

		SizeT separatorPos = string.find(argument.separator);

		if (separatorPos == std::string::npos) {
			return false;
		}

		nameString = string.substr(0, separatorPos);
		valueString = string.substr(separatorPos + 1, string.size() - 1);

	} else if (argument.unnamed) {

		valueString = string;
		nameString = argument.names[0];

	} else {

		// Side cases such as 'app.exe "--arg value"' are not meant to be valid

		nameString = string;

		if (argument.type != Argument::Flag) {
			valueString = arguments[cursor++];
		}

	}


	bool nameFound = std::find(argument.names.begin(), argument.names.end(), nameString) == argument.names.end();

	if (!argument.unnamed && nameFound) {
		return false;
	}


	if (argument.type == Argument::Flag) {

		for (const auto& name : argument.names) {
			storage.arguments.emplace_back(name, std::nullopt);
		}

	} else {

		std::optional<ValueT> value = parseValue(valueString, argument.type);

		if (!value) {
			return false;
		}


		for (const auto& name : argument.names) {
			storage.aliases.emplace_back(name, argument.names[0]);
		}

		storage.arguments.emplace_back(argument.names[0], *value);

	}


	storage.cursor = cursor;

	return true;

}


void ArgumentParser::evaluateNode(const Node& node, ParseLevel::Match& match, std::vector<ParseLevel>& levels, bool acceptOptional) {

	if (node.isParent()) {

		// Parent nodes cannot be parsed so propagate the match

		match = levels.back().match;

	} else if (parseArgument(node.argument, levels.back().state)) {

		// Argument matched so set levels dirty and set match to Full

		for (auto& level : levels) {
			level.dirty = true;
		}

		match = ParseLevel::Full;

	} else if (node.isOptional() && acceptOptional) {

		// Optional node not matching is a Partial match

		match = ParseLevel::Partial;

	} else if (match != ParseLevel::Partial || node.op != Node::Or) {

		// Not a full match attempt nor a new expression nulls the match

		match = ParseLevel::None;

	}

}

void ArgumentParser::evaluateUnorderedParent(const Node& node, ParseLevel::Match& match, std::vector<ParseLevel>& levels) {

	std::vector<Node> children = node.children;

	// Iterate over the parent's children as long as a full match is found and the vector is not empty

	do {

		// Reset match
		match = ParseLevel::Full;

		for (SizeT i = 0; const auto& child : children) {

			evaluateNode(child, match, levels, match != ParseLevel::None);

			if (match == ParseLevel::Full) {

				// Erase matching child
				children.erase(children.begin() + i);

				break;

			}

			i++;

		}

	} while (match == ParseLevel::Full && !children.empty());

}


ArgumentParser::StackOperation ArgumentParser::processParent(const ArgumentParser::Node& node, std::vector<ParseLevel>& levels) {

	SizeT argumentsCursor = 1; // Skip first argument (application path)


	if (!levels.empty()) {

		auto& level = levels.back();

		argumentsCursor = level.state.cursor;

		// Unrecoverable state (at lease for now)
		level.fatal |= (level.match == ParseLevel::None) && (node.op != Node::Or);

		// Unnecessary parent expansion
		bool unnecessary = (level.match == ParseLevel::Full) && (node.op == Node::Or);

		if (level.fatal || unnecessary) {
			return StackOperation::Pop;
		}

	}


	levels.emplace_back(ParseLevel::Full, argumentsCursor);


	if (!node.isUnordered()) {
		return StackOperation::Expand;
	}


	evaluateUnorderedParent(node, levels.back().match, levels);

	return StackOperation::None;

}

bool ArgumentParser::processExpandedParent(const Node& node, std::vector<ParseLevel>& levels) {

	auto& level = levels.back();


	if (levels.size() == 1) { // If Root

		// Unrecoverable state
		if (level.fatal || level.match == ParseLevel::None) {
			throw ArgumentParserException("Arguments not matching layout");
		}

		return true; // Root node reached, leave parsing loop

	}


	if (level.match == ParseLevel::Partial && level.dirty) {

		// Dirty partial match parent becomes a full match
		level.match = ParseLevel::Full;

	} else if (node.isOptional() && !level.dirty) {

		// Clean optional parent becomes a partial match
		level.match = ParseLevel::Partial;

	}


	return false;

}

void ArgumentParser::processNode(const Node& node, std::vector<ParseLevel>& levels, ParseLevel::Match& match) {

	if (match == ParseLevel::None) {

		if (node.op == Node::Or) {
			evaluateNode(node, match, levels);
		} else {
			levels.back().fatal = true; // Fatal (for now), beginning new expression without a match
		}

	} else if (match == ParseLevel::Full) {

		// Begin new expression
		if (node.op != Node::Or) {
			evaluateNode(node, match, levels);
		}

		// Else, full match has already been reached

	} else { // ParseLevel::MatchOpt

		// Seek for full match or begin new expression
		evaluateNode(node, match, levels);

	}

}


void ArgumentParser::parseTree() {

	std::vector<ParseLevel> levels; // Parsing state for each tree level

	std::stack<ParseNode> stack; // Ongoing nodes stack

	stack.emplace(layout.getRootNode());


	while (true) {

		auto& [node, expanded] = stack.top();

		if (node.isParent() && !expanded) {

			// First parent pass (pre-expansion), expand parent into the stack and process unordered parents
			// Unnecessary expansions are avoided to save performance

			StackOperation op = processParent(node, levels);

			if (op == StackOperation::Expand) {

				for (auto& child : std::views::reverse(node.children)) {
					stack.emplace(child);
				}

			} else if (op == StackOperation::Pop) {

				stack.pop();

				continue;

			}

			expanded = true;

			continue;

		}


		auto& level = levels.back();

		if (node.isParent()) {

			// Second parent pass (post-expansion), set up the parent to be treated like any other node

			if (processExpandedParent(node, levels)) {

				// Leave parsing loop when necessary
				break;

			}

		}


		// Current expression's match (if the current node is a parent, that will be in the level below)
		auto& match = node.isParent() ? levels[levels.size() - 2].match : level.match;

		if (!level.fatal || node.isParent()) {

			// Generic node pass, evaluate arguments when required and set match accordingly
			// Fatal arguments are skipped, parents still have to propagate their match to the level below

			processNode(node, levels, match);

		}


		if (node.isParent()) {

			// End parent, propagate state to the level below

			if (match != ParseLevel::None) {
				levels[levels.size() - 2].state.inherit(level.state);
			}

			// Pop tree level
			levels.pop_back();

		}


		stack.pop();

	}


	// Arguments matched, but not the entire vector has been used

	if (levels.back().state.cursor < arguments.size()) {
		throw ArgumentParserException("Arguments exceed layout");
	}


	// Push the root level's state into the class's maps for faster access

	for (const auto& [name, value] : levels.back().state.arguments) {

		bool success;

		if (value) {

			success = values.try_emplace(name, *value).second;

		} else {

			success = !values.contains(name);

			flags.emplace(name);

		}

		if (!success) {
			throw ArgumentParserException("Argument \"" + name + "\" has a duplicate");
		}

	}

	for (const auto& [alias, name] : levels.back().state.aliases) {

		if (!aliases.try_emplace(alias, name).second) {
			throw ArgumentParserException("Argument \"" + name + "\" has a duplicate");
		}

	}

}
