/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcrt.cpp
 */

#include "runtime.hpp"
#include "common/config.hpp"
#include "common/build.hpp"
#include "util/bits.hpp"
#include "util/log.hpp"
#include "util/destructionguard.hpp"
#include "common/exception.hpp"
#include "common/types.hpp"

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

	int main(int argc, char* argv[]) {

		constexpr u32 initReturnBase = 0x7000;

		DestructionGuard guard([]() {
			ArcRuntime::shutdown();
		});

		std::set_terminate([]() {
#ifndef ARC_ARCRT_SILENT
			LogE("Runtime") << "An unhandled exception has been thrown, terminating.";
#endif
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
				args.emplace_back(argv[i]);
			}

		} catch (const std::exception& e) {

#ifndef ARC_ARCRT_SILENT
			LogE("Runtime") << "Exception has been thrown before main.";
			LogE("Runtime") << e;
#endif
			showExceptionMessageBox(Exception::getMessage(e), Exception::getStackTrace(e));

			return initReturnBase + 1;

		}

		// Allow the developer to capture and analyze exceptions when running a Debug build

#if !ARC_DEBUG
		try {
#endif

			i32 ret = static_cast<i32>(arcMain(args));

			if (ret) {

#ifndef ARC_ARCRT_SILENT
				LogD("Runtime") << "Application exited with an error";
#endif
				return ret;

			}

#if !ARC_DEBUG
		} catch (const std::exception& e) {

#ifndef ARC_ARCRT_SILENT
			LogE("Runtime") << "The application has thrown an exception.";
			LogE("Runtime") << e;
#endif
			showExceptionMessageBox(Exception::getMessage(e), Exception::getStackTrace(e));

			return initReturnBase + 2;

		} catch (...) {

#ifndef ARC_ARCRT_SILENT
			LogE("Runtime") << "The application has thrown an exception of unknown type.";
#endif
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
			OS::MessageBox::create("Arclight - Application terminated", "The application has been terminated.\nAn unhandled exception has been thrown.\n" + ex + "\n" + st, OS::MessageBox::Type::Accept, OS::MessageBox::Icon::Error);
#endif

		} catch (...) {}

#endif

	}





bool ArcRuntime::initialize() noexcept {

#ifndef ARC_ARCRT_SILENT
	LogD("Runtime") << "Initializing runtime";
#endif

	//Platform init
	if (!ArcRuntime::platformInit()) {
		return false;
	}

	//Initialize Log
	Log::init();

#ifndef ARC_ARCRT_DISABLE_LOG_FILE
	Log::openLogFile(std::string(ARC_PATH_ROOT) + ARC_LOG_DIRECTORY);
#endif


#ifdef ARC_WINDOW_MODULE

	if (!glfwInit()) {
#ifndef ARC_ARCRT_SILENT
		LogE("ArcRT") << "Failed to initialize GLFW";
#endif
		return false;
	}

#endif

#ifdef ARC_FONT_MODULE

	if (!FontBackend::init()) {
#ifndef ARC_ARCRT_SILENT
		LogE("ArcRT") << "Failed to initialize font module";
#endif
		return false;
	}

#endif

#ifndef ARC_ARCRT_SILENT
	LogD("Runtime") << "Ready";
#endif

	return true;

}



void ArcRuntime::shutdown() noexcept {

#ifndef ARC_ARCRT_SILENT
	LogD("Runtime") << "Shutting down runtime";
#endif

#ifdef ARC_FONT_MODULE
	FontBackend::shutdown();
#endif

#ifdef ARC_WINDOW_MODULE
	glfwTerminate();
#endif

#ifndef ARC_ARCRT_SILENT
	LogD("Runtime") << "Bye";
#endif

#ifndef ARC_ARCRT_DISABLE_LOG_FILE
	Log::closeLogFile();
#endif

}

#endif
