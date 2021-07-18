#pragma once

#include "physics/physicsengine.h"
#include "render/physicsrenderer.h"
#include "input/inputsystem.h"
#include "acs/actormanager.h"

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
	InputHandler inputHandler;
	ActorManager manager;
	PhysicsEngine physicsEngine;
	PhysicsRenderer renderer;

};