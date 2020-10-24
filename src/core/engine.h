#pragma once

#include "core/audio/audioengine.h"
#include "core/input/inputsystem.h"
#include "core/window.h"
#include "util/profiler.h"


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
	void setupInputSystem();

	Profiler profiler;
	Window window;
	InputSystem inputSystem;
	InputHandler inputHandler;
	AudioEngine audioEngine;

};