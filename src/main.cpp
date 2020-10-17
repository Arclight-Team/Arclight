#include "util/log.h"
#include "util/vector.h"
#include "util/format.h"
#include "util/assert.h"
#include "util/matrix.h"
#include "util/file.h"
#include "util/profiler.h"
#include "util/random.h"
#include "core/window.h"
#include "core/input/inputsystem.h"
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
	window.setWindowConfig(WindowConfig().setResizable(true));
	bool windowCreated = window.create(200, 200, "KEKW");

	if (!windowCreated) {
		return -1;
	}
	
	Log::info("Core", "Window successfully created");

	InputSystem input;
	input.connect(window);
	InputContext& rootContext = input.createContext(0);
	rootContext.addState(0, InputContext::State(true, true, true, false));
	rootContext.addTrigger(0, KeyTrigger({ 48, 49, 50 }, KeyState::Pressed, KeyTriggerType::Continuous), 0);
	
	InputHandler handler;

	handler.setActionListener([](KeyAction action) {
		Log::info("Input Context", "Action triggered: %d", action);
		return true;
	});

	handler.setCharListener([](KeyChar character) {
		Log::info("Input Context", "Char obtained: %d", character);
		return true;
	});

	handler.setCursorListener([](double x, double y) {
		Log::info("Input Context", "Cursor at: %f, %f", x, y);
		return true;
	});

	handler.setScrollListener([](double x, double y) {
		Log::info("Input Context", "Scrolled: %f, %f", x, y);
		return true;
	});

	rootContext.linkHandler(handler);

	window.enableVSync();

	u32 i = 0;
	u32 c = 0;
	Timer timer;

	while (!window.closeRequested()) {

		window.swapBuffers();

		timer.start();
		window.pollEvents();
		c += timer.getElapsedTime();

		if (++i == 100) {
			Log::info("KEKW", "Average event time: %dus", c / 100);
			i = 0;
			c = 0;
		}

	}

	window.close();
	input.disconnect();
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