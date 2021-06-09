#pragma once

#include "types.h"


class BoxCollider;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class PhysicsTest {

public:
	PhysicsTest();
	~PhysicsTest();

	void init();
	void update();

	void addBoxCollider(BoxCollider& collider, u64 ownerID);

private:

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;


};