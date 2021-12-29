/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 bulletconv.hpp
 */

#pragma once

#include "worldtransform.hpp"
#include "math/vector.hpp"
#include "math/quaternion.hpp"

#include "LinearMath/btVector3.h"
#include "LinearMath/btTransform.h"


namespace Bullet {

    inline Vec3x fromBtVector3(const btVector3& v) {
        return Vec3x(v.x(), v.y(), v.z());
    }

    inline btVector3 fromVec3x(const Vec3x& v) {
        return btVector3(v.x, v.y, v.z);
    }

    inline QuatX fromBtQuaternion(const btQuaternion& q) {
        return QuatX(q.x(), q.y(), q.z(), q.w());
    }

    inline btQuaternion fromQuatX(const QuatX& q) {
        return btQuaternion(q.x, q.y, q.z, q.w);
    }

    inline WorldTransform fromBtTransform(const btTransform& t) {
        return WorldTransform(fromBtVector3(t.getOrigin()), fromBtQuaternion(t.getRotation()));
    }

    inline btTransform fromWorldTransform(const WorldTransform& t) {
        return btTransform(fromQuatX(t.rotation), fromVec3x(t.translation));
    }

}