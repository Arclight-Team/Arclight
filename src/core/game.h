#pragma once

#include "physics/physicstest.h"
#include <memory>


class Window;
class Renderer;

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
	PhysicsTest physicsSimulation;
	std::unique_ptr<Renderer> renderer;

};