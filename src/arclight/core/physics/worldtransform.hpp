#pragma once

#include "math/vector.hpp"
#include "math/quaternion.hpp"


struct WorldTransform {

    constexpr WorldTransform() : WorldTransform(Vec3x()) {}
    constexpr WorldTransform(const Vec3x& translation, const QuatX& rotation = QuatX()) : translation(translation), rotation(rotation) {}

    Vec3x translation;
    QuatX rotation;

};
