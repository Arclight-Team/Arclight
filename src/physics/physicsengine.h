#pragma once

#include "core/acs/component/boxcollider.h"
#include "core/acs/actor.h"
#include "util/profiler.h"
#include "types.h"


class ActorManager;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

class PhysicsEngine {

public:

	explicit PhysicsEngine(ActorManager& actorManager);
	~PhysicsEngine();

	void init(u32 ticksPerSecond);
	void update();

	void onBoxCreated(BoxCollider& collider, ActorID actor);
	void onBoxDestroyed(BoxCollider& collider, ActorID actor);

private:

	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	ActorManager& actorManager;
	
	Profiler profiler;
	Timer simTimer;
	u32 tps;

};