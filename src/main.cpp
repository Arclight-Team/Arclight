#include "util/log.h"
#include "util/vector.h"
#include "util/format.h"
#include "util/assert.h"
#include "util/matrix.h"
#include "util/file.h"
#include "util/profiler.h"
#include "util/random.h"
#include "core/window.h"
#include "core/event/event.h"

#include <GLFW/glfw3.h>


bool libraryStartup();
void libraryShutdown();


int main(){

	if (!libraryStartup()) {
		Log::error("Core", "Failed to initialize backend libraries, aborting");
		return -1;
	}

	Log::openLogFile();
	Log::info("Core", "Starting game engine");

	Profiler profiler(Timer::Unit::Milliseconds, 3);
	profiler.start();

	Window window;
	window.setWindowConfig(WindowConfig());
	bool windowCreated = window.create(200, 200, "KEKW");

	if (!windowCreated) {
		return -1;
	}
	
	Log::info("Core", "Window successfully created");

	window.enableVSync();

	window.setWindowMoveFunction([](Window* window, u32 x, u32 y) {
		window->setPosition(200, 200);
	});

	window.setWindowResizeFunction([](Window* window, u32 w, u32 h) {
		window->setSize(200, 200);
	});

	window.setWindowStateChangeFunction([](Window* window, WindowState state) {
		
		switch (state) {

			case WindowState::CloseRequest:
				window->dismissCloseRequest();
				break;

			case WindowState::Unfocused:
				window->requestAttention();
				break;

			case WindowState::Maximized:
			case WindowState::Minimized:
				window->restore();
				break;

		}

	});

	Timer timer;
	timer.start();

	while (!window.closeRequested()) {
		/*
		if (timer.getElapsedTime(Timer::Unit::Seconds) > 1) {
			
			switch (rand() % 9) {
				case 0:
					window.focus();
					break;
				case 1:
					window.hide();
					break;
				case 2:
					window.requestAttention();
					break;
				case 3:
					window.restore();
					break;
				case 4:
					window.maximize();
					break;
				case 5:
					window.show();
					break;
				case 6:
					window.minimize();
					break;
				case 7:
					window.setFullscreen();
					break;
				case 8:
					window.setWindowed();
					break;
			}

			timer.start();

		}
		
		
		if (timer.getElapsedTime(Timer::Unit::Milliseconds) > 1) {
			window.setPosition(rand() % 2000, rand() % 1400);
			timer.start();
		}
		*/

		window.swapBuffers();
		window.fetchEvents();

	}

	window.close();
	profiler.stop();
	Log::closeLogFile();

	libraryShutdown();

	return 0;

}




bool libraryStartup() {

	if (!glfwInit()) {
		Log::error("Core", "Failed to initialize GLFW");
		return false;
	}

	return true;

}



void libraryShutdown() {

	glfwTerminate();

}