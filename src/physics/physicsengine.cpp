#include "physicsengine.h"
#include "bulletconv.h"
#include "core/acs/actormanager.h"
#include "core/acs/component/rigidbody.h"
#include "util/log.h"
#include "types.h"

#include "btBulletDynamicsCommon.h"



void PhysicsEngine::init(u32 ticksPerSecond) {

	Log::info("Physics Engine", "Setting up simulation");

	createWorld(0);
	getWorld(0).setWorldGravity(Vec3x(0, 0, 0));
/*
	{ 
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);
		body->setRestitution(0.9);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);

	}
*/
	tps = ticksPerSecond;
	simTimer.start();

}



void PhysicsEngine::update() {

	profiler.start();

	double dt = simTimer.getElapsedTime(Time::Unit::Seconds);

	for(auto& [id, world] : dynamicWorlds) {
		world.simulate(dt, tps);
	}

	simTimer.start();

	profiler.stop("PhysicsSim");

	profiler.start();
	ComponentView view = actorManager.view<Transform, RigidBody>();

	for(auto [transform, rigidbody] : view) {

		WorldTransform rbwt = rigidbody.getTransform();
		WorldTransform owt = rigidbody.getTransformOffset();
		transform.position = rbwt.translation - owt.translation;
		transform.rotation = rbwt.rotation * owt.rotation.inverse();

	}

	profiler.stop("PhysicSync");

}



void PhysicsEngine::destroy() {

	dynamicWorlds.clear();

}



bool PhysicsEngine::createWorld(u32 worldID) {

	if(dynamicWorlds.contains(worldID)) {
		Log::warn("Physics Engine", "Attempted to add existing world with ID = %d", worldID);
		return false;
	}

	auto [it, success] = dynamicWorlds.try_emplace(worldID);

	if(!success) {
		Log::error("Physics Engine", "Failed to create dynamics world with ID = %d", worldID);
		return false;
	}

	it->second.create();
	return true;

}



void PhysicsEngine::destroyWorld(u32 worldID) {

	if(!dynamicWorlds.contains(worldID)) {
		Log::warn("Physics Engine", "Attempted to destroy non-existing world with ID = %d", worldID);
		return;
	}

	dynamicWorlds.erase(worldID);

}



DynamicsWorld& PhysicsEngine::getWorld(u32 worldID) {

	arc_assert(dynamicWorlds.contains(worldID), "Bad dynamics world access (ID = %d)", worldID);
	return dynamicWorlds[worldID];

}



void PhysicsEngine::onRigidBodyAdded(u32 worldID, RigidBody& body, ActorID actor) {

	arc_assert(dynamicWorlds.contains(worldID), "Bad dynamics world ID = %d", worldID);

	if(!body.isCreated()) {

		const Transform& transform = actorManager.getProvider().getComponent<Transform>(actor);
		body.create(WorldTransform(transform.position, transform.rotation), 1.0);

	}

	dynamicWorlds[worldID].addRigidBody(body);

}



void PhysicsEngine::onRigidBodyDeleted(u32 worldID, RigidBody& body) {

	arc_assert(dynamicWorlds.contains(worldID), "Bad dynamics world ID = %d", worldID);

	dynamicWorlds[worldID].deleteRigidBody(body);

	body.destroy();

}