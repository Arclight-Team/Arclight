#pragma once

#include "render/imagerenderer.h"
#include "input/inputsystem.h"
#include "util/profiler.h"
#include "util/ticker.h"

#include <vector>
#include <memory>



class Window;

class Game {

public:

	Game(Window& window);
	~Game();

	bool init();
	void update();
	void render();
	void destroy();

private:

	Window& window;
	InputSystem inputSystem;
	InputHandler rootInputHandler;
	InputHandler renderInputHandler;
	ImageRenderer imageRenderer;

	Profiler profiler;
	Ticker inputTicker;

};