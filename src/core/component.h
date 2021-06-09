#pragma once

#include <vector>
#include "types.h"
#include "util/assert.h"
#include "util/vector.h"
#include "memory/chunkallocator.h"


#define ARC_DOUBLE_PRECISION

class IComponent {};

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


class FakeTransform : public Transform {

public:
	u32 ok;

};


template<class C>
concept Component = std::is_base_of_v<IComponent, C>;


#define COMPONENT_REGISTER(type, id)			\
	if constexpr (std::is_same_v<C, type>) {	\
		return (id);							\
	}

#define COMPONENT_GET						\
	COMPONENT_REGISTER(Transform, 1)



class ComponentArray {

public:

	ComponentArray() {}
	

private:


};



class ComponentCollector {

public:

	ComponentCollector() {}

	/*template<Component C>
	IComponent& get() {
		return components[getComponentID<C>()][0];
	}*/

private:

	template<Component C>
	constexpr static u32 getComponentID() {
		COMPONENT_GET
	}

	std::vector<ComponentArray> components;

};