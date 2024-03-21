/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Stacktrace.hpp
 */

#pragma once

#include "Common/Types.hpp"
#include "Filesystem/Path.hpp"
#include "Common/Config.hpp"

#include <stacktrace>



#if defined(__cpp_lib_stacktrace) && !defined(ARC_CFG_DISABLE_STACKTRACES)
	#define __ARC_USE_STACKTRACES
#endif


class StacktraceEntry {

public:

	StacktraceEntry() noexcept = default;

#ifdef __ARC_USE_STACKTRACES
	explicit StacktraceEntry(const std::stacktrace_entry& entry) noexcept : eHandle(entry) {}
#else
	explicit StacktraceEntry(const auto& entry) noexcept {}
#endif


	std::string description() const {
#ifdef __ARC_USE_STACKTRACES
		return eHandle.description();
#else
		return "<Unknown>";
#endif
	}

	std::string filepath() const {
#ifdef __ARC_USE_STACKTRACES
		return eHandle.source_file();
#else
		return "<Unknown>";
#endif
	}

	SizeT lineNumber() const {
#ifdef __ARC_USE_STACKTRACES
		return eHandle.source_line();
#else
		return 0;
#endif
	}

private:

#ifdef __ARC_USE_STACKTRACES
	std::stacktrace_entry eHandle;
#endif

};



class Stacktrace {

public:

	Stacktrace() noexcept = default;

	explicit Stacktrace(SizeT start, SizeT maxDepth = -1) noexcept { record(start, maxDepth); }


	void record(SizeT start = 1, SizeT maxDepth = -1) noexcept {
#ifdef __ARC_USE_STACKTRACES
		stHandle = std::stacktrace::current(start, maxDepth);
#endif
	}

	bool empty() const noexcept {
#ifdef __ARC_USE_STACKTRACES
		return stHandle.empty();
#else
		return true;
#endif
	}

	SizeT depth() const noexcept {
#ifdef __ARC_USE_STACKTRACES
		return stHandle.size();
#else
		return 0;
#endif
	}

	StacktraceEntry entryAt(SizeT index) const {

#ifdef __ARC_USE_STACKTRACES
		if (index >= depth()) {
			return {};
		}

		return StacktraceEntry(stHandle[index]);
#else
		return {};
#endif

	}

	std::string toString() const {

#ifdef __ARC_USE_STACKTRACES
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
#else
		return "\tat ??? (<Unknown Source>)";
#endif

	}


	static Stacktrace here() {

		Stacktrace st;
		st.record();

		return st;

	}

private:

#ifdef __ARC_USE_STACKTRACES
	std::stacktrace stHandle;
#endif

};
