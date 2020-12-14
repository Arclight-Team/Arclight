#include "core/engine.h"
#include "util/file.h"
#include "util/log.h"



Engine::Engine() : profiler(Time::Unit::Seconds, 3) {}



bool Engine::initialize() {

	Uri::setApplicationUriRoot(Config::getUriRootPath());

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

	window.setFramebufferResizeFunction([this](u32 w, u32 h) {onFBResize(w, h); });

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
	renderTest.create(window.getFramebufferWidth(), window.getFramebufferHeight());

	//Disable V-Sync
	//window.disableVSync();

	//We're successfully running
	Log::info("Core", "Starting engine");

	//Start FPS tracker
	tracker.start();

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
	//audioEngine.playSound(":/sounds/rickroll.mp3");

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

		//Debug FPS
		window.setTitle(Config::getBaseWindowTitle() + " | FPS: " + std::to_string(tracker.getFPS()));

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

	window.setWindowConfig(WindowConfig().setResizable(true).setOpenGLVersion(3, 3));
	return window.create(Config::getDefaultWindowWidth(), Config::getDefaultWindowHeight(), Config::getBaseWindowTitle());

}



void Engine::setupInputSystem() {

	//Connect the input system to the window in order to receive events
	inputSystem.connect(window);

	//Create the root context and add actions to it
	InputContext& rootContext = inputSystem.createContext(0);
	rootContext.addState(0);

	rootContext.addAction(RenderTest::ActionID::CameraRotRight,		KeyTrigger({ 262 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraRotLeft,		KeyTrigger({ 263 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraRotDown,		KeyTrigger({ 264 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraRotUp,		KeyTrigger({ 265 }), true);	
	rootContext.addAction(RenderTest::ActionID::CameraMoveLeft,		KeyTrigger({ 65 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraMoveRight,	KeyTrigger({ 68 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraMoveBackward,	KeyTrigger({ 83 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraMoveForward,	KeyTrigger({ 87 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraMoveDown,		KeyTrigger({ 81 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraMoveUp,		KeyTrigger({ 69 }), true);
	rootContext.addAction(RenderTest::ActionID::CameraSpeedUp,		KeyTrigger({ 340 }, KeyState::Pressed), true);
	rootContext.addAction(RenderTest::ActionID::CameraSlowDown,		KeyTrigger({ 340 }, KeyState::Released), true);

	rootContext.addAction(RenderTest::ActionID::QuickScreenshot,	KeyTrigger({ 291 }), false);
	rootContext.addAction(RenderTest::ActionID::ReloadShaders,		KeyTrigger({ 292 }), false);
	rootContext.addAction(RenderTest::ActionID::ReloadResources,	KeyTrigger({ 293 }), false);
	rootContext.addAction(RenderTest::ActionID::ToggleDebug,		KeyTrigger({ 294 }), false);

	rootContext.registerAction(0, RenderTest::ActionID::CameraRotLeft);
	rootContext.registerAction(0, RenderTest::ActionID::CameraRotRight);
	rootContext.registerAction(0, RenderTest::ActionID::CameraRotDown);
	rootContext.registerAction(0, RenderTest::ActionID::CameraRotUp);
	rootContext.registerAction(0, RenderTest::ActionID::CameraMoveLeft);
	rootContext.registerAction(0, RenderTest::ActionID::CameraMoveRight);
	rootContext.registerAction(0, RenderTest::ActionID::CameraMoveBackward);
	rootContext.registerAction(0, RenderTest::ActionID::CameraMoveForward);
	rootContext.registerAction(0, RenderTest::ActionID::CameraMoveDown);
	rootContext.registerAction(0, RenderTest::ActionID::CameraMoveUp);
	rootContext.registerAction(0, RenderTest::ActionID::CameraSpeedUp);
	rootContext.registerAction(0, RenderTest::ActionID::CameraSlowDown);

	rootContext.registerAction(0, RenderTest::ActionID::QuickScreenshot);
	rootContext.registerAction(0, RenderTest::ActionID::ReloadShaders);
	rootContext.registerAction(0, RenderTest::ActionID::ReloadResources);
	rootContext.registerAction(0, RenderTest::ActionID::ToggleDebug);

	//Define input handler callbacks
	inputHandler.setCoActionListener([this](KeyAction action, double scale) {
		//Log::info("Input Context", "Action triggered: %d", action);
		renderTest.onKeyAction(action);
		return true;
	});

	inputHandler.setActionListener([this](KeyAction action) {
		//Log::info("Input Context", "Action triggered: %d", action);
		renderTest.onKeyAction(action);
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



void Engine::onFBResize(u32 w, u32 h) {
	renderTest.resizeWindowFB(w, h);
}