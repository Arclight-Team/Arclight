/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Main.cpp
 */

#include "Util/ArgumentParser.hpp"
#include "Util/Log.hpp"
#include "Candle/Core.hpp"



int main(int argc, char* argv[]) {

	constexpr const char* Layout = "<-h | --help> | {[-m | --module string] & [-v | --verbose] & [-s | --strict]}";

	constexpr const char* Usage = "test";


	ArgumentParser parser;


	try {

		parser.parse(argc, argv, Layout);

	} catch (const ArgumentParserException& ex) {

		LogE("Main") << "Failed to parse arguments";
		LogE("Main") << Usage;

		return 1;

	}


	if (parser.contains("--help")) {

		LogI("Main") << "Usage:";
		LogI("Main") << Usage;

		return 0;

	}


	constexpr const char* Compiler =
#ifdef ARC_COMPILER_MSVC
		"MSVC"
#elif defined(ARC_COMPILER_GCC)
		"GCC"
#elif defined(ARC_COMPILER_CLANG)
		"Clang"
#endif
	;

	constexpr const char* System =
#ifdef ARC_OS_WINDOWS
		"Windows"
#elif defined(ARC_OS_LINUX)
		"Linux"
#elif defined(ARC_OS_MACOS)
		"MacOS"
#endif
	;

	constexpr const char* Architecture =
#ifdef ARC_PLATFORM_X86
		"x86"
#elif ARC_PLATFORM_ARM
		"ARM"
#endif
	;

	LogI("Main") << "Arclight Candle - Build " ARC_PP_TIME " " ARC_PP_DATE;
	LogI("Main") << "Compiled with " << Compiler << " for " << System << " (" << Architecture << ")\n";


	Candle::ExecutionFlags flags{};

	if (parser.getFlag("--verbose")) {
		flags |= Candle::Verbose;
	}

	if (parser.getFlag("--strict")) {
		flags |= Candle::Strict;
	}

	return !Candle::execute(parser.getString("--module", ""), flags);

}
