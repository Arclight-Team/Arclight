#include "game.h"
#include "window.h"
#include "render/physicsrenderer.h"
#include "util/optionalref.h"
#include "util/sparsearray.h"
#include "util/any.h"
#include "debug.h"
#include "acs/actorblueprints.h"
#include "util/profiler.h"
#include "acs/componentview.h"
#include "acs/component/transform.h"
#include "acs/component/model.h"
#include "acs/component/boxcollider.h"


Game::Game(Window& window) : window(window) {}

Game::~Game() {}


bool Game::init() {

	window.disableVSync();

	physicsSimulation.init();
	renderer.init();
/*
	SparseArray<int> array;
	array.add(3, 324);
	array.add(6, 20);
	array.add(1, 30);
	array.add(2, 30);
	SparseArray<int>::Iterator a = array.begin();

	for(a; a != array.end(); a++){
		Log::info("", "%d %d", *a, 5);
	}
*/
	manager.setup();
	manager.registerActor<ExampleActor>(0);
	manager.registerActor<BoxActor>(1);

	manager.addObserver<Transform>([](Transform& transform, ActorID actor){
		Log::info("Observer", "Got Transform for actor %d", actor);
	});

	manager.addObserver<BoxCollider>([](BoxCollider& box, ActorID actor){
		Log::info("Observer", "Got BoxCollider for actor %d", actor);
	});
	
	for(u32 i = 0; i < 32; i++) {
		manager.spawn(1, Transform(Vec3x(i, 0, 0)));
	}

	return true;

}



void Game::update() {

	physicsSimulation.update();

	ArcDebug() << "Starting frame";

	ComponentView view = manager.view<Transform, BoxCollider>();

	for(const auto& [transform, collider] : view) {
		ArcDebug() << transform.position;
	}

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