/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcrt.cpp
 */

#include "arcrt.hpp"
#include "arcconfig.hpp"
#include "arcbuild.hpp"
#include "util/bits.hpp"
#include "util/log.hpp"
#include "util/destructionguard.hpp"
#include "common/exception.hpp"
#include "types.hpp"

#ifdef ARC_WINDOW_MODULE
	#include <GLFW/glfw3.h>
#endif

#ifdef ARC_FONT_MODULE
	#include "font/fontbackend.hpp"
#endif


#include "os/messagebox.hpp"


namespace ArcRuntime {
	extern bool platformInit();
}


#ifdef ARC_USE_ARCRT

	extern u32 arcMain(const std::vector<std::string>& args);
	static void showExceptionMessageBox(const std::string& ex, const std::string& st) noexcept;

	int main(int argc, char** argv) {

		constexpr u32 initReturnBase = 0x7000;

		DestructionGuard guard([]() {
			ArcRuntime::shutdown();
		});

		std::set_terminate([]() {
			LogE("Runtime") << "An unhandled exception has been thrown, terminating.";
			showExceptionMessageBox("Unhandled Exception", "<Untraceable>");
			ArcRuntime::shutdown();
		});

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

			LogE("Runtime") <<"Exception has been thrown before main.";
			Exception::print(e);
			Exception::printStackTrace(e);
			showExceptionMessageBox(Exception::getMessage(e), Exception::getStackTrace(e));

			return initReturnBase + 1;

		}

		// Allow the developer to capture and analyze exceptions when running a Debug build

#if !ARC_DEBUG
		try {
#endif

			i32 ret = static_cast<i32>(arcMain(args));

			if (ret) {

				LogE("Runtime") << "Application exited with an error";
				return ret;

			}

#if !ARC_DEBUG
		} catch (const std::exception& e) {

			LogE("Runtime") << "The application has thrown an exception.";
			Exception::print(e);
			Exception::printStackTrace(e);
			showExceptionMessageBox(Exception::getMessage(e), Exception::getStackTrace(e));

			return initReturnBase + 2;

		} catch (...) {

			LogE("Runtime") << "The application has thrown an exception of unknown type.";
			showExceptionMessageBox("Object-Type Exception", "<Untraceable>");

			return initReturnBase + 3;

		}
#endif

		return 0;

	}


	static void showExceptionMessageBox(const std::string& ex, const std::string& st) noexcept {

#ifdef ARC_SHOW_CRASH_MESSAGEBOX

		try {

#ifdef ARC_OS_WINDOWS
			MessageBox::create("Arclight - Application terminated", "The application has been terminated.\nAn unhandled exception has been thrown.\n" + ex + "\n" + st, MessageBox::Type::Accept, MessageBox::Icon::Error);
#endif

		} catch (...) {}

#endif

	}





bool ArcRuntime::initialize() noexcept {

	LogI("Runtime") << "Initializing runtime";

	//Platform init
	if (!ArcRuntime::platformInit()) {
		return false;
	}

	//Initialize Log
	Log::init();
	Log::openLogFile(std::string(ARC_PATH_ROOT) + ARC_LOG_DIRECTORY);


#ifdef ARC_WINDOW_MODULE

	if (!glfwInit()) {
		LogE("ArcRT") << "Failed to initialize GLFW";
		return false;
	}

#endif

#ifdef ARC_FONT_MODULE

	if (!FontBackend::init()) {
		LogE("ArcRT") << "Failed to initialize font module";
		return false;
	}

#endif

	LogI("Runtime") << "Ready";

	return true;

}



void ArcRuntime::shutdown() noexcept {

	LogI("Runtime") << "Shutting down runtime";

#ifdef ARC_FONT_MODULE
	FontBackend::shutdown();
#endif

#ifdef ARC_WINDOW_MODULE
	glfwTerminate();
#endif

	LogI("Runtime") << "Bye";
	Log::closeLogFile();

}

#endif
