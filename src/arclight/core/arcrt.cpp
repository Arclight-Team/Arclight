/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcrt.cpp
 */

#include "arcrt.hpp"
#include "arcconfig.hpp"
#include "util/bits.hpp"
#include "util/log.hpp"
#include "types.hpp"

#ifdef ARC_WINDOW_MODULE
	#include <GLFW/glfw3.h>
#endif



#ifdef ARC_USE_ARCRT

	extern u32 arcMain(const std::vector<std::string>& args);

	struct ArcRTShutdownGuard {

		~ArcRTShutdownGuard() noexcept {
			ArcRuntime::shutdown();
		}

	};

	int main(int argc, char** argv) {

		constexpr u32 initReturnBase = 0x7000;
		[[maybe_unused]] ArcRTShutdownGuard guard;

		if (!ArcRuntime::initialize()) {
			return initReturnBase;
		}

		std::vector<std::string> args;

		try {

			args.reserve(argc);

			for (u32 i = 0; i < argc; i++) {
				args.emplace_back(*(argv + i));
			}

		} catch (const std::exception& e) {

			Log::error("Runtime", "Exception has been thrown before main: %s", e.what());
			return initReturnBase + 1;

		}

		try {

			i32 ret = Bits::cast<i32>(arcMain(std::move(args)));

			if (ret) {

				Log::error("Runtime", "Application exited with an error");
				return ret;

			}

		} catch (const std::exception& e) {

			Log::error("Runtime", "The application has thrown an exception: %s", e.what());
			return initReturnBase + 2;

		}

		return 0;

	}

#endif



bool ArcRuntime::initialize() noexcept {

	//Initialize Log
	Log::init();
	Log::openLogFile(std::string(ARC_URI_ROOT) + "/" + ARC_LOG_DIRECTORY);

	Log::info("Runtime", "Initializing runtime");

#ifdef ARC_WINDOW_MODULE

	if (!glfwInit()) {
		Log::error("ArcRT", "Failed to initialize GLFW");
		return false;
	}

#endif

	Log::info("Runtime", "Ready");

	return true;

}



void ArcRuntime::shutdown() noexcept {

	Log::info("Runtime", "Shutting down runtime");

#ifdef ARC_WINDOW_MODULE
	glfwTerminate();
#endif

	Log::info("Runtime", "Bye");
	Log::closeLogFile();

}