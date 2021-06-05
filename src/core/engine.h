#pragma once

#include "util/fpstracker.h"
#include "window.h"
#include "game.h"


class Engine {

public:

	Engine();

	bool initialize();
	void run();
	void shutdown();

private:

	bool initializeBackend();
	void shutdownBackend();
	bool createWindow();

	Game game;
	FPSTracker tracker;
	Window window;
	
};