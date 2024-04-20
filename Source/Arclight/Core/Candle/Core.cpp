/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Core.cpp
 */

#include "Core.hpp"
#include "Util/DestructionGuard.hpp"

#include <stack>
#include <ranges>



static constexpr const char* LogName = "Candle";


Candle::Detail::TreeNode* Candle::Detail::root = nullptr;


std::vector<std::string> Candle::Detail::ExecuteNode::process(const std::string& spacing, bool& terminate, Candle::ExecutionFlags flags) const {

	terminate = false;


	bool prefix = false;

	std::vector<std::string> report;

	for (SizeT i = 1; const auto& test : node.tests) {

		Candle::TestContext context;


		try {
			test.function(context);
		} catch (const std::exception& ex) {
			context.error("(Uncaught) " + Exception::getMessage(ex));
		} catch (...) {
			context.error("(Uncaught) Unknown exception");
		}


		std::string name = test.unnamed() ? std::format("Test #{}", i) : test.name;


		constexpr const char* Status[] = {"Fail", "Pass"};

		const bool success = context.success();

		LogI(LogName) << std::format("{}| [{}/{} {}] {}", spacing, i++, node.tests.size(), Status[success], name);

		if (success) {
			continue;
		}


		if (!prefix) {

			report.emplace_back("");
			report.emplace_back("> " + module);

			prefix = true;

		}

		report.emplace_back(std::format("+ In '{}'", name));

		for (const auto& failure : context.data()) {
			report.emplace_back("| \t" + failure.toString(bool(flags & Verbose)));
		}


		if (bool(flags & Strict)) {
			terminate = true;
			break;
		}

	}


	return report;

}



bool Candle::execute(const std::string& module, ExecutionFlags flags) noexcept {

	if (module.starts_with('.') || module.ends_with('.')) {
		LogE(LogName) << "Cannot execute illegal module '" << module << "'";
		return false;
	}


	const bool rootTarget = module.empty();

	SizeT targetDepth = rootTarget ? 0 : -1;

	if (rootTarget) {
		LogI(LogName) << "Executing all modules";
		LogI(LogName) << "";
	}


	std::vector<std::string> report;


	Detail::ExecuteNode(*Detail::root, "").traverse(rootTarget, [&](const Detail::ExecuteNode& current, SizeT depth) {

		if (targetDepth == -1) {

			if (current.module != module) {
				return true;
			}

			targetDepth = depth;

			LogI(LogName) << "Executing module '" << module << "'";
			LogI(LogName) << "";

		} else if (depth - 1 < targetDepth) {

			// Leave traverse once processing is finished

			return false;

		}


		const std::string spacing(depth - targetDepth, '\t');

		// Isolate last module name (e.g. Core.Math -> Math)

		SizeT pos = current.module.rfind('.');
		pos = (pos == std::string::npos) ? 0 : pos + 1;

		LogI(LogName) << spacing << "> " << current.module.substr(pos);


		bool terminate = false;

		std::vector<std::string> output = current.process(spacing, terminate, flags);

		report.insert(report.begin(), output.begin(), output.end());

		return !terminate;

	});

	if (targetDepth == -1) {
		LogE(LogName) << "Module '" << module << "' does not exist";
		return false;
	}


	const bool success = report.empty();


	LogI(LogName) << "";

	if (success) {
		LogI(LogName) << "Tests succeded!";
	} else {
		LogE(LogName) << "Tests failed!";
	}

	for (const auto& line : report) {
		LogE(LogName) << line;
	}


	if constexpr (__TIME__[7] & 3) {
		LogI(LogName) << "";
		LogI(LogName) << "~ The wax has melted";
	}


	return success;

}

bool Candle::registerModule(const std::string& module, const TestHandle& test) noexcept {

	if (module.starts_with('.') || module.ends_with('.') || module.empty()) {
		LogE(LogName) << "Cannot register test at illegal module '" << module << "'";
		return false;
	}

	if (!test.function) {
		LogE(LogName) << "Cannot register nullptr as test function";
		return false;
	}


	std::vector<std::string> names;


	std::string string = module;

	SizeT pos = 0;

	while (true) {

		pos = string.find('.', pos);

		if (pos == std::string::npos) {
			names.emplace_back(string);
			break;
		}

		names.emplace_back(string.substr(0, pos++));

	}


	Detail::TreeNode* node = Detail::root;

	for (SizeT i = 0; const auto& name : names) {

		if (!node->children.contains(name)) {
			node->children.emplace(name, Detail::TreeNode{});
		}

		node = &node->children.at(name);

		if (++i == names.size() && node->isParent()) {
			LogE(LogName) << "Cannot register test at '" << module << "', conflicts found at '" << name << "'";
			return false;
		}

	}

	node->tests.emplace_back(test);


	return true;

}


Candle::Detail::StaticRegister::StaticRegister(const std::string& module, const TestHandle& test) noexcept {

	static DestructionGuard guard([]() {
		delete root;
	});

	if (!root) {
		root = new TreeNode;
	}

	std::ios_base::Init ios; // Guarantees Log availability at dynamic initialization

	if (!registerModule(module, test)) {
		LogE(LogName) << "Static registration failed, exiting";
		arc_exit(1);
	}

}
