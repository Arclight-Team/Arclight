#include "physicsengine.h"
#include "bulletconv.h"
#include "core/acs/actormanager.h"
#include "core/acs/component/boxcollider.h"
#include "core/acs/component/rigidbody.h"
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



void PhysicsEngine::init(u32 ticksPerSecond) {

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
		body->setRestitution(0.9);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);

	}

	tps = ticksPerSecond;
	simTimer.start();

}



void PhysicsEngine::update() {

	profiler.start();

	double dt = simTimer.getElapsedTime(Time::Unit::Seconds);
	dynamicsWorld->stepSimulation(dt, 1, 1.0 / tps);
	
	simTimer.start();

	profiler.stop("PhysicsSim");

	profiler.start();
	ComponentView view = actorManager.view<Transform, RigidBody>();

	for(auto [transform, rigidbody] : view) {

		WorldTransform rbwt = rigidbody.getTransform();
		WorldTransform owt = rigidbody.getTransformOffset();
		transform.position = rbwt.translation - owt.translation;
		transform.rotation = rbwt.rotation;// * owt.rotation.inverse();

	}

	profiler.stop("PhysicSync");

}



void PhysicsEngine::onBoxCreated(BoxCollider& collider, ActorID actor) {

	const Transform& transform = actorManager.getProvider().getComponent<Transform>(actor);
	btCollisionShape* box = new btBoxShape(Bullet::fromVec3x(collider.size / 2.0));

	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(Bullet::fromVec3x(transform.position));

	btScalar mass(1.0f);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);

	if (isDynamic) {
		box->calculateLocalInertia(mass, localInertia);
	}

	btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, box, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setRestitution(2);
	body->setDamping(0, 0);
	body->setFriction(0);
	//body->applyCentralImpulse(btVector3(0, -200, 0));

	dynamicsWorld->addRigidBody(body);
	collider.handle = body;

}



void PhysicsEngine::onRigidBodyAdded(RigidBody& body, ActorID actor) {

	if(!body.isCreated()) {

		const Transform& transform = actorManager.getProvider().getComponent<Transform>(actor);
		body.create(WorldTransform(transform.position, transform.rotation), 1.0);

	}

	dynamicsWorld->addRigidBody(static_cast<btRigidBody*>(body.handle));

}