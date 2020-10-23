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
	bool windowCreated = window.create(200, 200, "Among Us");

	if (!windowCreated) {
		return -1;
	}
	
	Log::info("Core", "Window successfully created");

	InputSystem input;
	input.connect(window);
	InputContext& rootContext = input.createContext(0);
	rootContext.addState(0);
	rootContext.addBoundAction(0, KeyTrigger({ 48, 49, 50 }), KeyTrigger({ 48 }));
	rootContext.addAction(1, KeyTrigger({ 51 }), true);
	rootContext.addAction(2, KeyTrigger({ 52 }), true);
	rootContext.registerAction(0, 0);
	rootContext.registerAction(0, 1);
	rootContext.registerAction(0, 2);
	rootContext.restoreBinding(0);

	InputHandler handler;

	handler.setCoActionListener([](KeyAction action, double scale) {
		Log::info("Input Context", "Action triggered: %d", action);
		return true;
	});

	handler.setActionListener([](KeyAction action) {
		Log::info("Input Context", "Action triggered: %d", action);
		return true;
	});

	handler.setKeyListener([](Key key, KeyState state) {
		//Log::info("Input Context", "Key triggered: %d, %d", key, state);
		return true;
	});

	handler.setCharListener([](KeyChar character) {
		//Log::info("Input Context", "Char obtained: %d", character);
		return true;
	});

	handler.setCursorListener([](double x, double y) {
		//Log::info("Input Context", "Cursor at: %f, %f", x, y);
		return true;
	});

	handler.setScrollListener([](double x, double y) {
		//Log::info("Input Context", "Scrolled: %f, %f", x, y);
		return true;
	});

	rootContext.linkHandler(handler);

	window.enableVSync();

	Timer timer;
	timer.start();
	u64 lastTime = timer.getElapsedTime();
	u64 accum = 0;
	u32 ticks = 0;

	while (!window.closeRequested()) {

		u64 delta = timer.getElapsedTime() - lastTime;
		lastTime = timer.getElapsedTime();
		accum += delta;

		if (accum > 16667) {
			ticks = accum / 16667;
			accum %= 16667;
		}

		window.pollEvents();
		input.updateContinuous(ticks);

		window.swapBuffers();

		ticks = 0;

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