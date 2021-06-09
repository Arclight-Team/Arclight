#include "game.h"
#include "window.h"
#include "render/physicsrenderer.h"
#include "physics/boxcollider.h"
#include "component.h"
#include "util/optionalref.h"
#include "sparsearray.h"


Game::Game(Window& window) : window(window) {}

Game::~Game() {}


bool Game::init() {

	window.disableVSync();

	physicsSimulation.init();
	renderer.init();
	ComponentCollector coll;

	SparseArray<int> array;
	array.add(3, 324);
	OptionalRef<int> ref = array.get(3);
	Log::info("", "%d %d", ref.has(), ref.get());

	return true;

}



void Game::update() {

	physicsSimulation.update();

}



void Game::render() {

	renderer.render();

}



void Game::destroy() {
	renderer.destroy();
}



void Game::addCube(float size, const Vec3f& pos, const Vec3f rot, const Vec3f& scale) {

	static u64 currentID = 0;

	Object object{
		ObjectType::Cube,
		currentID,
		pos,
		rot,
		scale,
		new BoxCollider(size, size, size)
	};

	physicsSimulation.addBoxCollider(*static_cast<BoxCollider*>(object.collider), object.id);

	objects.push_back(object);
	currentID++;

}