#include "physicstest.h"
#include "util/log.h"
#include "types.h"
#include "boxcollider.h"

#include "btBulletDynamicsCommon.h"


PhysicsTest::PhysicsTest() : collisionConfiguration(nullptr), dispatcher(nullptr), overlappingPairCache(nullptr), solver(nullptr), dynamicsWorld(nullptr) {}

PhysicsTest::~PhysicsTest() {

	if (collisionConfiguration) {delete collisionConfiguration;}
	if (dispatcher) {delete dispatcher;}
	if (overlappingPairCache) {delete overlappingPairCache;}
	if (solver) {delete solver;}
	if (dynamicsWorld) {delete  dynamicsWorld;}

}




void PhysicsTest::init() {

	Log::info("Physics Test", "Setting up simulation");

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -10, 0));

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

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

}



void PhysicsTest::update() {

	dynamicsWorld->stepSimulation(1 / 60.0, 10);

	for (u32 i = 0; i < dynamicsWorld->getNumCollisionObjects(); i++) {

		const auto& collider = dynamicsWorld->getCollisionObjectArray()[i];
		btVector3 pos = collider->getWorldTransform().getOrigin();
		//Log::info("Physics Test", "Position: %f, %f, %f", pos.getX(), pos.getY(), pos.getZ());

	}

}



void PhysicsTest::addBoxCollider(BoxCollider& collider, u64 ownerID) {

	btCollisionShape& boxShape = *collider.getInternalCollider();

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0, -56, 0));

	btScalar mass(0.);

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		boxShape.calculateLocalInertia(mass, localInertia);

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, &boxShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setUserPointer(reinterpret_cast<void*>(ownerID));

	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);
	//dynamicsWorld->addCollisionObject
}
