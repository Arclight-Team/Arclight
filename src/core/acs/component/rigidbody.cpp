#include "rigidbody.h"
#include "physics/bulletconv.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "LinearMath/btDefaultMotionState.h"



constexpr static btRigidBody* rbcast(void* handle) {
    return static_cast<btRigidBody*>(handle);
}


constexpr static const btRigidBody* rbcast(const void* handle) {
    return static_cast<const btRigidBody*>(handle);
}


RigidBody::RigidBody(const WorldTransform& transform, double mass) {
    create(transform, mass);
}


RigidBody::~RigidBody() {
    destroy();
}



void RigidBody::create(const WorldTransform& transform, double mass) {

	btCollisionShape* box = new btBoxShape(Bullet::fromVec3x(Vec3x(1 / 2.0)));

	btTransform rbTransform = Bullet::fromWorldTransform(transform);
	btVector3 localInertia(0, 0, 0);

	if (mass != 0.0) {
		box->calculateLocalInertia(mass, localInertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(rbTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, box, localInertia);
	btRigidBody* rb = new btRigidBody(rbInfo);

    handle = rb;

}



void RigidBody::destroy() {

   if(isCreated()) {

        btRigidBody* body = rbcast(handle);
        delete body->getCollisionShape();
        delete body->getMotionState();
        delete body;
        handle = nullptr;

    }

}



RigidBody::RigidBody(RigidBody&& body) noexcept : transformOffset(std::move(body.transformOffset)), handle(std::exchange(body.handle, nullptr)) {}



RigidBody& RigidBody::operator=(RigidBody&& body) noexcept {

    transformOffset = std::move(body.transformOffset);
    handle = std::exchange(body.handle, nullptr);

    return *this;

}



bool RigidBody::isCreated() const {
    return handle;
}


WorldTransform RigidBody::getTransform() const {

    btTransform t;
    rbcast(handle)->getMotionState()->getWorldTransform(t);
    return Bullet::fromBtTransform(t);

}


Vec3x RigidBody::getTransformOffset() const noexcept {
    return transformOffset;
}



void RigidBody::applyImpulse(const Vec3x& direction) {
    rbcast(handle)->applyCentralImpulse(Bullet::fromVec3x(direction));
}



void RigidBody::setRestitution(double restitution) {
    rbcast(handle)->setRestitution(restitution);
}



void RigidBody::setMass(double mass) {

    auto* rb = rbcast(handle);
    btVector3 inertia(0, 0, 0);
    
    if(mass != 0.0) {
        rb->getCollisionShape()->calculateLocalInertia(mass, inertia);
    }

    rb->setMassProps(mass, inertia);

}



void RigidBody::setGravity(const Vec3x& gravity) {
    rbcast(handle)->setGravity(Bullet::fromVec3x(gravity));
}



void RigidBody::setTransformOffset(const Vec3x& offset) noexcept {
    transformOffset = offset;
}