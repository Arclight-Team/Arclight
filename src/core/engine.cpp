#include "core/engine.h"
#include "util/file.h"
#include "util/log.h"
#include "render/gle/gc.h"


Engine::Engine() : profiler(Timer::Unit::Seconds, 3) {}



bool Engine::initialize() {

	//Open up log file
	Log::openLogFile();
	Log::info("Core", "Setting up engine");

	//Initialize backend
	if (!initializeBackend()) {
		Log::error("Core", "Library backend initialization failed");
		return false;
	}

	//Start engine profiler
	profiler.start();

	//Create the main window
	if (!createWindow()) {
		Log::error("Core", "Failed to create window");
		return false;
	}

	Log::info("Core", "Window successfully created");

	//Setup input system and input handler
	setupInputSystem();
	Log::info("Core", "Input system successfully set up");

	//Initialize audio engine
	if (!audioEngine.initialize()) {
		Log::error("Core", "Audio engine failed to initialize");
		return false;
	}

	Log::info("Core", "Audio engine initialized");

	//Create render test
	renderTest.create();

	//Enable V-Sync
	window.enableVSync();

	//We're successfully running
	Log::info("Core", "Starting engine");

	return true;

}



void Engine::run() {

	//Create the tick timer
	Timer timer;
	timer.start();

	//Variables to store current tick state
	u64 lastTime = timer.getElapsedTime();
	u64 accum = 0;

	//Play sounds
	audioEngine.playSound(":/sounds/rickroll.mp3");

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
		inputSystem.updateContinuous(ticks);

		//Update audio engine
		audioEngine.update();

		//Run render test
		renderTest.run();

		//Swap render buffers
		window.swapBuffers();

	}

}



void Engine::shutdown() {

	Log::info("Core", "Shutting down engine");

	//Close instances
	renderTest.destroy();
	audioEngine.shutdown();
	inputSystem.disconnect();
	window.close();

	//Stop the profiler and get engine runtime
	profiler.stop();

	Log::info("Core", "Shutting down backend");

	//Shutdown engine backend libraries
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

	window.setWindowConfig(WindowConfig().setResizable(true));
	return window.create(200, 200, "Among Us");

}



void Engine::setupInputSystem() {

	//Connect the input system to the window in order to receive events
	inputSystem.connect(window);

	//Create the root context and add actions to it
	InputContext& rootContext = inputSystem.createContext(0);
	rootContext.addState(0);
	rootContext.addBoundAction(0, KeyTrigger({ 48, 49, 50 }), KeyTrigger({ 48 }));
	rootContext.addAction(1, KeyTrigger({ 51 }), true);
	rootContext.addAction(2, KeyTrigger({ 52 }), true);
	rootContext.addAction(3, KeyTrigger({ 0 }));
	rootContext.addAction(4, KeyTrigger({ 1 }), true);
	rootContext.registerAction(0, 3);
	rootContext.registerAction(0, 4);
	rootContext.restoreBinding(0);

	//Define input handler callbacks
	inputHandler.setCoActionListener([this](KeyAction action, double scale) {
		Log::info("Input Context", "Action triggered: %d", action);
		return true;
	});

	inputHandler.setActionListener([this](KeyAction action) {
		Log::info("Input Context", "Action triggered: %d", action);
		this->audioEngine.playSound(URI(":/sounds/world1.wav"));
		return true;
	});

	inputHandler.setKeyListener([this](Key key, KeyState state) {
		//Log::info("Input Context", "Key triggered: %d, %d", key, state);
		return true;
	});

	inputHandler.setCharListener([this](KeyChar character) {
		//Log::info("Input Context", "Char obtained: %d", character);
		return true;
	});

	inputHandler.setCursorListener([this](double x, double y) {
		//Log::info("Input Context", "Cursor at: %f, %f", x, y);
		return true;
	});

	inputHandler.setScrollListener([this](double x, double y) {
		//Log::info("Input Context", "Scrolled: %f, %f", x, y);
		return true;
	});

	//Link handler to the root context
	rootContext.linkHandler(inputHandler);

}