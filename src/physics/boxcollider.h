#pragma once

#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "collider.h"


class BoxCollider : public Collider {

public:

	BoxCollider(float w, float h, float d) {}

	virtual btCollisionShape* getInternalCollider() override {
		return _collider;
	}

private:

	btBoxShape* _collider;

};