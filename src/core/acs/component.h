#pragma once

#include <type_traits>


class IComponent {};

typedef u32 ComponentTypeID;

template<class C>
concept Component = std::is_base_of_v<IComponent, C>;


struct ComponentID {

	template<Component C>
	constexpr ComponentTypeID getComponentTypeID() {
		static_assert(false, "Component not registered");
		return 0;
	}

};


#define REGISTER_COMPONENT(c, id)									\
template<>															\
constexpr ComponentTypeID ComponentID::getComponentTypeID<c>() {	\
	return id;														\
}





/*
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
*/