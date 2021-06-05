#include "game.h"
#include "window.h"
#include "render/physicsrenderer.h"


Game::Game(Window& window) : window(window), renderer(std::make_unique<PhysicsRenderer>()) {}

Game::~Game() {}


bool Game::init() {

	physicsSimulation.init();
	renderer->init();

	return true;

}



void Game::update() {

	physicsSimulation.update();

}



void Game::render() {

	renderer->render();

}



void Game::destroy() {
	renderer->destroy();
}