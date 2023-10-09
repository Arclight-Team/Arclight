/*
 *	 Copyright (c) 2023 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 argumentparser.cpp
 */

#pragma once

#include "argumentparser.hpp"
#include "util/assert.hpp"

#include <stack>


ArgumentParser::ArgumentParser() : argumentsCursor(1) {}

ArgumentParser::ArgumentParser(const std::vector<std::string>& args, ArgumentLayout layout) : argumentsCursor(1), arguments(args), layout(std::move(layout)) {}

ArgumentParser::ArgumentParser(int argc, char* argv[], ArgumentLayout layout) : argumentsCursor(1), layout(std::move(layout)) {
	setArguments(argc, argv);
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

	setArguments(argc, argv);

	parse();

}

void ArgumentParser::reset() {

	argumentsCursor = 1;

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


bool ArgumentParser::parseArgument(const Argument& arg) {

	SizeT cursor = argumentsCursor;

	SizeT req = arg.stringsCount();

	if (req > arguments.size() - cursor) {
		return false;
	}


	std::string argString = arguments[cursor++];

	std::string nameString;
	std::string valueString;

	if (arg.separator != ' ') {

		// Split argument string with the separator character

		SizeT separatorPos = argString.find(arg.separator);

		if (separatorPos == std::string::npos) {
			return false;
		}

		nameString = argString.substr(0, separatorPos);
		valueString = argString.substr(separatorPos + 1, argString.size() - 1);

	} else if (arg.unnamed) {

		valueString = argString;
		nameString = arg.names[0];

	} else {

		// Side cases such as 'app.exe "--arg value"' are not meant to be valid

		nameString = argString;

		if (arg.type != Argument::Flag) {
			valueString = arguments[cursor++];
		}

	}


	if (!arg.unnamed && std::find(arg.names.begin(), arg.names.end(), nameString) == arg.names.end()) {
		return false;
	}


	if (arg.type == Argument::Flag) {

		for (const auto& name : arg.names) {

			if (!flags.insert(name).second && !aliases.contains(name)) {
				throw ArgumentParserException("Name \"" + name + "\" already inserted");
			}

		}

		argumentsCursor = cursor;

		return true;

	}


	for (const auto& name : arg.names) {

		if (!aliases.try_emplace(name, arg.names[0]).second) {
			throw ArgumentParserException("Name \"" + name + "\" already inserted");
		}

	}


	ValueT value;

	switch (arg.type) {

		case Argument::Int:		value = parseArithmetic<i64>(valueString, nameString);		break;
		case Argument::UInt:	value = parseArithmetic<u64>(valueString, nameString);		break;
		case Argument::Float:	value = parseArithmetic<float>(valueString, nameString);	break;
		case Argument::Double:	value = parseArithmetic<double>(valueString, nameString);	break;

		case Argument::Word:

			if (valueString.find(' ') != std::string::npos) {
				throw ArgumentParserException("Argument \"" + nameString + "\" has invalid value");
			}

			[[fallthrough]];

		case Argument::String:

			value = valueString;

			break;

		default:
			arc_force_assert("Illegal argument type");

	}

	values.try_emplace(arg.names[0], value);


	argumentsCursor = cursor;

	return true;

}


void ArgumentParser::parseTree() {

	std::vector<ParseLevel> levels; // Dirty and parsing state for each parent's level

	std::stack<ParseNode> stack; // Parsing nodes stack

	stack.emplace(layout.rootNode);


	auto updateState = [&](const Node& node, ParseLevel::State& mainState) {

		if (node.isParent()) {

			// Parent nodes cannot be parsed unlike argument nodes:
			// - Propagate its level into the previous one

			mainState = levels.back().state;

		} else if (parseArgument(node.argument)) {

			// If argument node parse is successful:
			// - Set Match state
			// - Dirty all previous levels

			mainState = ParseLevel::Match;

			for (auto& level : levels) {
				level.dirty = true;
			}

		} else {

			// If argument node parse fails:
			// - Set MatchOpt state if the node is optional or if the operator is Or (match not strictly required)
			// - Set Error state otherwise

			bool optional = (node.type == Node::Optional || node.op == Node::Or);

			mainState = optional ? ParseLevel::MatchOpt : ParseLevel::Error;

		}

	};


	// Parse the layout tree with a depth-first postorder traverse

	while (true) {

		auto& [node, expanded] = stack.top();

		if (node.isParent() && !expanded) {

			levels.emplace_back(ParseLevel::Match);

			for (auto& child : std::views::reverse(node.children)) {
				stack.emplace(child);
			}

			expanded = true;

			continue;

		}


		auto& [state, dirty] = levels.back();


		// Premature parent node checks
		// TODO Further optimizations

		bool rootLevel = levels.size() == 1;

		if (node.isParent()) {

			if (state == ParseLevel::MatchOpt && dirty) {

				// Dirty MatchOpt parent node counts as Match

				state = ParseLevel::Match;

			} else if (state == ParseLevel::Error && (rootLevel || (!node.isOptional() && dirty))) {

				// Mandatory dirty parent node in Error is unrecoverable

				throw ArgumentParserException("Arguments not matching layout");

			} else if (node.isOptional() && !dirty) {

				// Clean optional parent node counts as MatchOpt

				state = ParseLevel::MatchOpt;

			}

			// Leave from root section

			if (rootLevel) {
				break;
			}

		}


		// Common node state update and error checking

		auto& mainState = node.isParent() ? levels[levels.size() - 2].state : state; // Current expression level state

		if (mainState == ParseLevel::Error) {

			// If our node is in Error we have to:
			// - State update if the operator is Or (hopefully we get a Match/MatchOpt)
			// - Else, throw if the node is dirty or root (something unnecessary got matched or fatal error in root)

			if (node.op == Node::Or) {
				updateState(node, mainState);
			} else if (rootLevel || dirty) {
				throw ArgumentParserException("Arguments not matching layout");
			}

		} else if (mainState == ParseLevel::Match) {

			// If our node is in Match we have to:
			// - State update if the operator is not Or (we are starting a new expression)
			// - Else, throw if the node is parent and dirty (something unnecessary got matched)

			if (node.op != Node::Or) {
				updateState(node, mainState);
			} else if (node.isParent() && dirty) {
				throw ArgumentParserException("Arguments not matching layout");
			}

		} else if (mainState == ParseLevel::MatchOpt) {

			// If our node is in MatchOpt we have to update the state if:
			// - The node is not parent (complete Match attempt otherwise keep MatchOpt)
			// - The operator is not Or (we are starting a new expression)
			// - Parent is not in error (continue normally)

			if (!node.isParent() || node.op != Node::Or || state != ParseLevel::Error) { // TODO Verify `&& !dirty` requirement
				updateState(node, mainState);
			}

		}

		if (node.isParent()) {
			levels.pop_back();
		}

		stack.pop();

	}


	if (argumentsCursor < arguments.size()) {
		throw ArgumentParserException("Arguments exceed layout");
	}

}
