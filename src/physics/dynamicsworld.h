#pragma once

#include "util/vector.h"
#include <memory>



class RigidBody;

class DynamicsWorld {

public:

    constexpr DynamicsWorld() = default;

    void create();

    void setWorldGravity(const Vec3x& gravity);

    void addRigidBody(RigidBody& body);
    void deleteRigidBody(RigidBody& body);

    void simulate(double dt, u32 tps);

private:

    std::shared_ptr<struct DynamicsWorldConfiguration> config;

};