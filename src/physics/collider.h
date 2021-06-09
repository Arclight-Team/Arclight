#pragma once

#include "BulletCollision/CollisionShapes/btEmptyShape.h"


class Collider {

public:

	virtual btCollisionShape* getInternalCollider() {
		return &defaultCollider;
	};

private:

	static inline btEmptyShape defaultCollider;

};