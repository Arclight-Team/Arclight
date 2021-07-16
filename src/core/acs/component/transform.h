#pragma once

#include "component.h"
#include "util/vector.h"


class Transform : public IComponent {

public:

	constexpr Transform() : Transform(Vec3x(0)) {}
	constexpr Transform(const Vec3x& position, const Vec3x& rotation = Vec3x(0), const Vec3x& scale = Vec3x(1)) : position(position), rotation(rotation), scale(scale) {}

	Vec3x position;
	Vec3x rotation; //ok use QuatX later
	Vec3x scale;

};

REGISTER_COMPONENT(Transform, 0)