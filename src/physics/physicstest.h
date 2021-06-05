#pragma once

#include "LinearMath/btAlignedObjectArray.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"


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

private:

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	btAlignedObjectArray<btCollisionShape*> collisionShapes;

};