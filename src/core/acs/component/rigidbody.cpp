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
	btRigidBody* rb = new btRigidBody(mass, motionState, box, localInertia);

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



RigidBody::RigidBody(RigidBody&& body) : transformOffset(std::move(body.transformOffset)), handle(std::exchange(body.handle, nullptr)) {}



RigidBody& RigidBody::operator=(RigidBody&& body) {

    transformOffset = std::move(body.transformOffset);
    handle = std::exchange(body.handle, nullptr);

    return *this;

}



bool RigidBody::isCreated() const {
    return handle;
}


WorldTransform RigidBody::getTransform() const {
    return Bullet::fromBtTransform(rbcast(handle)->getWorldTransform());
}



WorldTransform RigidBody::getInterpolatedTransform() const {

    btTransform t;
    rbcast(handle)->getMotionState()->getWorldTransform(t);
    return Bullet::fromBtTransform(t);

}



WorldTransform RigidBody::getTransformOffset() const {
    return transformOffset;
}



Vec3x RigidBody::getLinearVelocity() const {
    return Bullet::fromBtVector3(rbcast(handle)->getLinearVelocity());
}



Vec3x RigidBody::getAngularVelocity() const {
    return Bullet::fromBtVector3(rbcast(handle)->getAngularVelocity());
}



void RigidBody::activate() {
    rbcast(handle)->activate(true);
}



void RigidBody::deactivate() {
    rbcast(handle)->setActivationState(0);
}



void RigidBody::applyForce(const Vec3x& direction) {
    rbcast(handle)->applyCentralForce(Bullet::fromVec3x(direction));
}



void RigidBody::applyForce(const Vec3x& direction, const Vec3x& contactOffset) {
    rbcast(handle)->applyForce(Bullet::fromVec3x(direction), Bullet::fromVec3x(contactOffset));
}



void RigidBody::applyImpulse(const Vec3x& direction) {
    rbcast(handle)->applyTorqueImpulse(Bullet::fromVec3x(direction));
}



void RigidBody::applyImpulse(const Vec3x& direction, const Vec3x& contactOffset) {
    rbcast(handle)->applyImpulse(Bullet::fromVec3x(direction), Bullet::fromVec3x(contactOffset));
}



void RigidBody::applyTorque(const Vec3x& torque) {
    rbcast(handle)->applyTorque(Bullet::fromVec3x(torque));
}



void RigidBody::applyTorqueImpulse(const Vec3x& torque) {
    rbcast(handle)->applyTorqueImpulse(Bullet::fromVec3x(torque));
}



void RigidBody::clearForces() {
    rbcast(handle)->clearForces();
}



void RigidBody::disableGravity() {
    setGravity(Vec3x(0));
}



void RigidBody::disableFriction() {
    setLinearDamping(0);
    setAngularDamping(0);
}



void RigidBody::setLinearVelocity(const Vec3x& velocity) {
    rbcast(handle)->setLinearVelocity(Bullet::fromVec3x(velocity));
}



void RigidBody::setAngularVelocity(const Vec3x& velocity) {
    rbcast(handle)->setAngularVelocity(Bullet::fromVec3x(velocity));
}



void RigidBody::setLinearDamping(double damping) {
    rbcast(handle)->setDamping(damping, rbcast(handle)->getAngularDamping());
}



void RigidBody::setAngularDamping(double damping) {
    rbcast(handle)->setDamping(rbcast(handle)->getLinearDamping(), damping);
}



void RigidBody::setLinearScale(const Vec3x& scale) {
    rbcast(handle)->setLinearFactor(Bullet::fromVec3x(scale));
}



void RigidBody::setAngularScale(const Vec3x& scale) {
    rbcast(handle)->setAngularFactor(Bullet::fromVec3x(scale));
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



void RigidBody::setTransformOffset(const WorldTransform& offset) {
    transformOffset = offset;
}