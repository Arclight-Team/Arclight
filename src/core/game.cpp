#include "game.h"
#include "window.h"
#include "render/physicsrenderer.h"
#include "physics/boxcollider.h"
#include "util/optionalref.h"
#include "util/sparsearray.h"
#include "acs/componentprovider.h"
#include "util/any.h"
#include "util/concepts.h"
#include "util/arcdebug.h"


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
		Log::info("", "%d", *a);
	}

	struct K{
		K() = default;
		K(const K&) = default;
	};

	if constexpr (CopyConstructible<const K&>) {
		ArcDebug() << "uh yea";
	}

	if constexpr (std::is_copy_constructible_v<K>){
		ArcDebug() << "ok?";
	}
	K k;
	Any<20> any(k);


	ComponentProvider provider;

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