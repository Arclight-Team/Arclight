#pragma once

#include "physics/physicstest.h"
#include "render/physicsrenderer.h"
#include <vector>
#include <memory>



class Collider;

enum class ObjectType {
	Cube,
	Cuboid
};


struct Object {

	ObjectType type;
	u64 id;
	Vec3f position;
	Vec3f rotation;		//TODO: Quaternion
	Vec3f scale;
	Collider* collider;

};



class Window;

class Game {

public:

	Game(Window& window);
	~Game();

	bool init();
	void update();
	void render();
	void destroy();

	void addCube(float size, const Vec3f& pos, const Vec3f rot, const Vec3f& scale);

private:

	Window& window;
	PhysicsTest physicsSimulation;
	PhysicsRenderer renderer;
	std::vector<Object> objects;

};