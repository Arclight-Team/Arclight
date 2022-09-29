/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 stacktrace.hpp
 */

#pragma once

#include "types.hpp"
#include "filesystem/path.hpp"
#include "arcconfig.hpp"

#include <stacktrace>



class StacktraceEntry {

public:

	StacktraceEntry() noexcept = default;
	explicit StacktraceEntry(const std::stacktrace_entry& entry) noexcept : eHandle(entry) {}

	std::string description() const {
		return eHandle.description();
	}

	std::string filepath() const {
		return eHandle.source_file();
	}

	SizeT lineNumber() const {
		return eHandle.source_line();
	}

private:

	std::stacktrace_entry eHandle;

};



class Stacktrace {

public:

	Stacktrace() noexcept = default;
	explicit Stacktrace(SizeT start, SizeT maxDepth = -1) noexcept { record(start, maxDepth); }

	void record(SizeT start = 1, SizeT maxDepth = -1) noexcept {
#ifndef ARC_DISABLE_STACKTRACES
		stHandle = std::stacktrace::current(start, maxDepth);
#endif
	}

	bool empty() const noexcept {
		return stHandle.empty();
	}

	SizeT depth() const noexcept {
		return stHandle.size();
	}

	StacktraceEntry entryAt(SizeT index) const {

		if (index >= depth()) {
			return StacktraceEntry();
		}

		return StacktraceEntry(stHandle[index]);

	}

	std::string toString() const {

#ifdef ARC_DISABLE_STACKTRACES

		return "\tat ??? (<Unknown Source>)";

#else
		std::string trace;

		for (SizeT i = 0; i < depth(); i++) {

			StacktraceEntry entry = entryAt(i);

			std::string desc = entry.description();
			std::string file = entry.filepath();
			SizeT line = entry.lineNumber();

			Path path(file);
			std::string fileDecl = path.exists() ? "file" : "unknown file";

			trace += "\tat ";
			trace += desc.empty() ? "<Unnamed Symbol>" : desc;
			trace += " (";
			trace += file.empty() || line == 0 ? "<Unknown Source>" : "in " + fileDecl + " " + file + " at line " + std::to_string(entry.lineNumber());
			trace += ")";

			if (i < (depth() - 1)) {
				trace += "\n";
			}

		}

		return trace;

#endif

	}


	static Stacktrace here() {

		Stacktrace st;
		st.record();

		return st;

	}

private:

	std::stacktrace stHandle;

};