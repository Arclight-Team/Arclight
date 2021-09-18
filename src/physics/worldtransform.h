#pragma once

#include "math/vector.h"
#include "math/quaternion.h"


struct WorldTransform {

    constexpr WorldTransform() : WorldTransform(Vec3x()) {}
    constexpr WorldTransform(const Vec3x& translation, const QuatX& rotation = QuatX()) : translation(translation), rotation(rotation) {}

    Vec3x translation;
    QuatX rotation;

};
