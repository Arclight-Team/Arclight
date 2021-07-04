#include "game.h"
#include "window.h"
#include "render/physicsrenderer.h"
#include "physics/boxcollider.h"
#include "util/optionalref.h"
#include "util/sparsearray.h"
#include "util/any.h"
#include "debug.h"
#include "acs/actor/exampleactor.h"
#include "util/profiler.h"
#include "acs/componentview.h"
#include "acs/component/transform.h"


Game::Game(Window& window) : window(window) {}

Game::~Game() {}


bool Game::init() {

	window.disableVSync();

	physicsSimulation.init();
	renderer.init();

	SparseArray<int> array;
	array.add(3, 324);
	array.add(6, 20);
	array.add(1, 30);
	array.add(2, 30);
	SparseArray<int>::Iterator a = array.begin();

	for(a; a != array.end(); a++){
		Log::info("", "%d %d", *a, 5);
	}

	manager.setup();
	manager.registerActor<ExampleActor>(0);
	
	Transform t{};

	for(u32 i = 0; i < 20; i++) {
		t.position += Vec3i(1, 1, 1);
		manager.spawn(0, t);
	}

	ComponentView view = manager.view<Transform>();

	for(const auto& [transform] : view) {
		ArcDebug() << transform.position.x << transform.position.y << transform.position.z;
	}

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
/*
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
*/
}