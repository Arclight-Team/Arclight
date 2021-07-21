#pragma once

#include "util/vector.h"
#include "component.h"


class WorldTransform;

class RigidBody : public IComponent {

public:

    constexpr RigidBody() : handle(nullptr) {}
    RigidBody(const WorldTransform& transform, double mass);
    ~RigidBody();

    RigidBody(const RigidBody& body) noexcept = delete;
    RigidBody& operator=(const RigidBody& body) noexcept = delete;
    RigidBody(RigidBody&& body) noexcept;
    RigidBody& operator=(RigidBody&& body) noexcept;

    void create(const WorldTransform& transform, double mass);
    void destroy();

    bool isCreated() const;

    WorldTransform getTransform() const;
    Vec3x getTransformOffset() const noexcept;

    void applyImpulse(const Vec3x& direction);

    void setRestitution(double restitution);
    void setMass(double mass);
    void setGravity(const Vec3x& gravity);
    void setTransformOffset(const Vec3x& offset) noexcept;

private:

    friend class PhysicsEngine;
    
    Vec3x transformOffset;
    void* handle;

};