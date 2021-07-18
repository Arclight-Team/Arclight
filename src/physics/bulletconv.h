#pragma once

#include "LinearMath/btVector3.h"
#include "util/vector.h"


namespace Bullet {

    inline Vec3x fromBtVector3(const btVector3& v) {
        return Vec3x(v.x(), v.y(), v.z());
    }

    inline btVector3 fromVec3x(const Vec3x& v) {
        return btVector3(v.x, v.y, v.z);
    }

}