#include "core/engine.h"
#include "util/file.h"
#include "util/log.h"
#include "util/matrix.h"


Engine::Engine() : game(window) {}



bool Engine::initialize() {

	Uri::setApplicationUriRoot(Config::getUriRootPath());

	//Open up log file
	Log::init();
	Log::openLogFile();
	Log::info("Core", "Setting up engine");

	//Initialize backend
	if (!initializeBackend()) {
		Log::error("Core", "Library backend initialization failed");
		return false;
	}

	//Create the main window
	if (!createWindow()) {
		Log::error("Core", "Failed to create window");
		return false;
	}

	Log::info("Core", "Window successfully created");

	//Initialize game
	if (!game.init()) {
		Log::error("Core", "Failed to initialize game core");
		return false;
	}

	//We're successfully running
	Log::info("Core", "Engine up and running");

	return true;

}



void Engine::run() {

	//Create the tick timer
	Timer timer;
	timer.start();

	//Variables to store current tick state
	u64 lastTime = timer.getElapsedTime();
	u64 accum = 0;

	//Loop until window close event is requested
	while (!window.closeRequested()) {

		//Calculate delta time and add to accumulator
		u64 delta = timer.getElapsedTime() - lastTime;
		lastTime = timer.getElapsedTime();
		accum += delta;
		u32 ticks = 0;

		//Get tick count on accumulator overflow
		if (accum > 16667) {
			ticks = accum / 16667;
			accum %= 16667;
		}

		//Update window and input system
		window.pollEvents();

		//Update game
		game.update();

		//Render game
		game.render();

		//Swap render buffers
		window.swapBuffers();

		//Debug FPS
		window.setTitle(Config::getBaseWindowTitle() + " | FPS: " + std::to_string(tracker.getFPS()));

	}

}



void Engine::shutdown() {

	//Shut down engine
	Log::info("Core", "Shutting down engine");
	game.destroy();

	//Close instances
	window.close();

	//Shut down engine backend libraries
	Log::info("Core", "Shutting down backend");
	shutdownBackend();

	Log::info("Core", "Bye");

	//Finally close log file
	Log::closeLogFile();

}



bool Engine::initializeBackend() {
	return Window::initialize();
}



void Engine::shutdownBackend() {
	Window::shutdown();
}



bool Engine::createWindow() {

	window.setWindowConfig(WindowConfig().setResizable(true).setOpenGLVersion(3, 3));
	return window.create(Config::getDefaultWindowWidth(), Config::getDefaultWindowHeight(), Config::getBaseWindowTitle());

}