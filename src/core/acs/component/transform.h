#pragma once

#include "component.h"
#include "util/vector.h"


class Transform : public IComponent {

public:

#ifdef ARC_DOUBLE_PRECISION
	Vec3d position;
	Vec3d rotation; //ok use QuatD later
	Vec3d scale;
#else
	Vec3f position;
	Vec3f rotation; //ok use QuatF later
	Vec3f scale;
#endif

};