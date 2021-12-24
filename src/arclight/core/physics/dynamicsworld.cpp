#include "dynamicsworld.hpp"
#include "bulletconv.hpp"
#include "btBulletDynamicsCommon.h"



struct DynamicsWorldConfiguration {

	enum class Type {
		Default
	};

	DynamicsWorldConfiguration(Type type = Type::Default) {

		collisionConfiguration = new btDefaultCollisionConfiguration();
		dispatcher = new btCollisionDispatcher(collisionConfiguration);
		overlappingPairCache = new btDbvtBroadphase();
		solver = new btSequentialImpulseConstraintSolver();
		world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	}

	~DynamicsWorldConfiguration() {

		for (int i = world->getNumConstraints() - 1; i >= 0; i--) {
			auto constraint = world->getConstraint(i);

			world->removeConstraint(constraint);

			delete constraint;
		}

		for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			auto object = world->getCollisionObjectArray()[i];
			//auto* rb = btRigidBody::upcast(object);

			//if (rb && rb->getMotionState()) {
			//	while (rb->getNumConstraintRefs()) {
			//		auto* constraint = rb->getConstraintRef(0);
			//		world->removeConstraint(constraint);
			//		delete constraint;
			//	}

			//	delete rb->getMotionState();
			//	delete rb->getCollisionShape();
			//	world->removeRigidBody(rb);

			//	/*RigidBody* rbc = static_cast<RigidBody*>(rb->getUserPointer());
			//	if (rbc) {
			//		rbc->destroy();
			//	}*/

			//	delete object;
			//}
			//else {
				world->removeCollisionObject(object);
				delete object;
			//}

		}

		delete world;
		delete solver;
		delete overlappingPairCache;
		delete dispatcher;
		delete collisionConfiguration;

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


/*
void DynamicsWorld::addRigidBody(RigidBody& body) {
	btRigidBody* rb = static_cast<btRigidBody*>(body.handle);
	rb->setUserPointer(config->world);
	config->world->addRigidBody(rb);
}



void DynamicsWorld::deleteRigidBody(RigidBody& body) {
	config->world->removeRigidBody(static_cast<btRigidBody*>(body.handle));
}
*/


void DynamicsWorld::simulate(double dt, u32 tps) {
	config->world->stepSimulation(dt, 1, 1.0 / tps);
}