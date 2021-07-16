#pragma once

#include "collider.h"
#include "util/vector.h"



class BoxCollider : public Collider {

public:

    constexpr BoxCollider(const Vec3x& size) : Collider(Type::Box), size(size) {}

    Vec3x size;

};

REGISTER_COMPONENT(BoxCollider, 2)