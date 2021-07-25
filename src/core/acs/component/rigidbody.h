#pragma once

#include "util/vector.h"
#include "physics/worldtransform.h"
#include "component.h"


struct WorldTransform;

class RigidBody : public IComponent {

public:

    constexpr RigidBody() : handle(nullptr) {}
    RigidBody(const WorldTransform& transform, double mass);
    ~RigidBody();

    RigidBody(const RigidBody& body) = delete;
    RigidBody& operator=(const RigidBody& body) = delete;
    RigidBody(RigidBody&& body);
    RigidBody& operator=(RigidBody&& body);

    void create(const WorldTransform& transform, double mass);
    void destroy();

    bool isCreated() const;

    WorldTransform getTransform() const;
    WorldTransform getTransformOffset() const;
    Vec3x getLinearVelocity() const;
    Vec3x getAngularVelocity() const;

    void activate();
    void deactivate();
    void applyForce(const Vec3x& direction);
    void applyForce(const Vec3x& direction, const Vec3x& contactOffset);
    void applyImpulse(const Vec3x& direction);
    void applyImpulse(const Vec3x& direction, const Vec3x& contactOffset);
    void applyTorque(const Vec3x& torque);
    void applyTorqueImpulse(const Vec3x& torque);
    void clearForces();
    void disableGravity();
    void disableFriction();

    void setLinearVelocity(const Vec3x& velocity);
    void setAngularVelocity(const Vec3x& velocity);
    void setLinearDamping(double damping);
    void setAngularDamping(double damping);
    void setLinearScale(const Vec3x& scale);
    void setAngularScale(const Vec3x& scale);
    void setRestitution(double restitution);
    void setMass(double mass);
    void setGravity(const Vec3x& gravity);
    void setTransformOffset(const WorldTransform& offset);

private:

    friend class DynamicsWorld;
    friend class PhysicsEngine;
    
    WorldTransform transformOffset;
    void* handle;

};