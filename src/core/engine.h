#pragma once

#include "audio/audioengine.h"
#include "input/inputsystem.h"
#include "core/window.h"
#include "render/rendertest.h"
#include "util/profiler.h"
#include "util/fpstracker.h"


class Engine {

public:

	Engine();

	bool initialize();
	void run();
	void shutdown();

private:

	void onFBResize(u32 w, u32 h);

	bool initializeBackend();
	void shutdownBackend();
	bool createWindow();
	void setupInputSystem();

	Profiler profiler;
	Window window;
	InputSystem inputSystem;
	InputHandler inputHandler;
	AudioEngine audioEngine;
	RenderTest renderTest;
	FPSTracker tracker;

};