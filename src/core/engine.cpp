#include "core/engine.h"
#include "arcconfig.h"
#include "config.h"
#include "util/log.h"
#include "util/file.h"
#include "image/bmp.h"
#include "stream/fileinputstream.h"


Engine::Engine() : game(window) {}



bool Engine::initialize() {

	Uri::setSpecialUriHandler(':', []() { return Config::getUriAssetPath(); });

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

	window.setAlwaysOnTop(false);

	return true;

}



void Engine::run() {

	//Start FPS tracker
	tracker.start();

	//Create the tick timer
	Timer timer;
	timer.start();

	//Loop until window close event is requested
	while (!window.closeRequested()) {

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
	 
	if(!window.create(Config::getDefaultWindowWidth(), Config::getDefaultWindowHeight(), Config::getBaseWindowTitle())) {
		return false;
	}

#ifdef ARC_APP_ICON_ENABLE
	File iconFile(":/logo.bmp", File::In | File::Binary);

	if(iconFile.open()) {

		FileInputStream iconStream(iconFile);
		Image<Pixel::RGBA8> iconImage = BMP::loadBitmap<Pixel::RGBA8>(iconStream);
		iconImage.resize(ImageScaling::Bilinear, 64, 64);
		iconImage.flipY();
		window.setIcon(iconImage);

	} else {

		Log::warn("Engine", "Failed to set window icon");

	}
#endif

	return true;

}