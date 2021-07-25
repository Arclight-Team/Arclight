#include "dynamicsworld.h"

#include "bulletconv.h"
#include "core/acs/component/rigidbody.h"
#include "btBulletDynamicsCommon.h"



struct DynamicsWorldConfiguration {

	enum class Type {
		Default
	};

	DynamicsWorldConfiguration(Type type = Type::Default) {

		collisionConfiguration = new btDefaultCollisionConfiguration;
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase;
		solver = new btSequentialImpulseConstraintSolver;
		world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	}

	~DynamicsWorldConfiguration() {

		delete collisionConfiguration;
		delete dispatcher;
		delete overlappingPairCache;
		delete solver;
		delete world;

	}

    btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	btDynamicsWorld* world;

};



void DynamicsWorld::create() {
    config = std::make_shared<DynamicsWorldConfiguration>();
}



void DynamicsWorld::setWorldGravity(const Vec3x& gravity) {
	config->world->setGravity(Bullet::fromVec3x(gravity));
}



void DynamicsWorld::addRigidBody(RigidBody& body) {
	config->world->addRigidBody(static_cast<btRigidBody*>(body.handle));
}



void DynamicsWorld::deleteRigidBody(RigidBody& body) {
	config->world->removeRigidBody(static_cast<btRigidBody*>(body.handle));
}



void DynamicsWorld::simulate(double dt, u32 tps) {
	config->world->stepSimulation(dt, 1, 1.0 / tps);

}