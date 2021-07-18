#include "physicsengine.h"
#include "bulletconv.h"
#include "core/acs/actormanager.h"
#include "util/log.h"
#include "types.h"

#include "btBulletDynamicsCommon.h"


PhysicsEngine::PhysicsEngine(ActorManager& actorManager) : collisionConfiguration(nullptr), dispatcher(nullptr), overlappingPairCache(nullptr), solver(nullptr), dynamicsWorld(nullptr), actorManager(actorManager) {}

PhysicsEngine::~PhysicsEngine() {

	if (dynamicsWorld) {delete dynamicsWorld;}
	if (collisionConfiguration) {delete collisionConfiguration;}
	if (dispatcher) {delete dispatcher;}
	if (overlappingPairCache) {delete overlappingPairCache;}
	if (solver) {delete solver;}

}



void PhysicsEngine::init() {

	Log::info("Physics Engine", "Setting up simulation");

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

	tickTimer.start();

	//Variables to store current tick state
	lastTickTime = static_cast<u64>(tickTimer.getElapsedTime());
	tickAccumulator = 0;

}



void PhysicsEngine::update() {

	//Calculate delta time and add to accumulator
	u64 delta = static_cast<u64>(tickTimer.getElapsedTime()) - lastTickTime;
	lastTickTime = static_cast<u64>(tickTimer.getElapsedTime());
	tickAccumulator += delta;
	u32 ticks = 0;

	//Get tick count on accumulator overflow
	if (tickAccumulator > 16667) {
		ticks = tickAccumulator / 16667;
		tickAccumulator %= 16667;
	}

	for(u32 i = 0; i < ticks; i++) {

		dynamicsWorld->stepSimulation(1 / 60.0f, 10);

		ComponentView view = actorManager.view<Transform, BoxCollider>();

		for(auto [transform, collider] : view) {

			btRigidBody* body = static_cast<btRigidBody*>(collider.handle);
			btTransform rbtransform;
			body->getMotionState()->getWorldTransform(rbtransform);

			transform.position = Bullet::fromBtVector3(rbtransform.getOrigin());

			btScalar rx, ry, rz;
			rbtransform.getRotation().getEulerZYX(rz, ry, rx);
			transform.rotation = Vec3x(rx, ry, rz);

		}

	}

}



void PhysicsEngine::onBoxCreated(BoxCollider& collider, ActorID actor) {

	const Transform& transform = actorManager.getProvider().getComponent<Transform>(actor);
	btCollisionShape* box = new btBoxShape(Bullet::fromVec3x(collider.size));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(Bullet::fromVec3x(transform.position));

	btScalar mass(0.1f);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic) {
		box->calculateLocalInertia(mass, localInertia);
	}

	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, box, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	dynamicsWorld->addRigidBody(body);
	collider.handle = body;

}



void PhysicsEngine::onBoxDestroyed(BoxCollider& collider, ActorID actor) {

	btRigidBody* body = static_cast<btRigidBody*>(collider.handle);

	delete body->getCollisionShape();
	delete body->getMotionState();
	delete body;

	body = nullptr;

}