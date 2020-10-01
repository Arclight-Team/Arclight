#include "util/log.h"
#include "util/vector.h"
#include "util/format.h"
#include "util/assert.h"
#include "util/matrix.h"
#include "util/file.h"
#include "util/profiler.h"
#include "util/random.h"
#include "core/input/window.h"
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

	window.enableVSync();

	for (u32 i = 0; i < window.getMonitorCount(); i++) {
		Monitor m = window.getMonitor(i);
		Log::info("Core", "Monitor %d (%s):\nSize: %d x %d (%dmm x %dmm)\n", i, m.name.c_str(), m.width, m.height, m.physicalW, m.physicalH);
	}

	Timer timer;
	timer.start();

	while (!window.closeRequested()) {

		window.swapBuffers();
		window.pollEvents();

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