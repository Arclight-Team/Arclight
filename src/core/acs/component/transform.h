#pragma once

#include "component.h"
#include "math/vector.h"
#include "math/quaternion.h"


class Transform : public IComponent {

public:

	constexpr Transform() : Transform(Vec3x(0)) {}
	constexpr Transform(const Vec3x& position, const QuatX& rotation = QuatX(), const Vec3x& scale = Vec3x(1)) : position(position), rotation(rotation), scale(scale) {}

	Vec3x position;
	Vec3x scale;
	QuatX rotation;

};