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
	rootContext.addState(0);
	rootContext.addTrigger(0, KeyTrigger({ 48, 49, 50 }, KeyState::Pressed, KeyTriggerType::Continuous), 0);

	window.enableVSync();

	while (!window.closeRequested()) {

		window.swapBuffers();
		window.pollEvents();

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