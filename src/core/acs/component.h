#pragma once

#include <type_traits>


class IComponent {};

template<class C>
concept Component = std::is_base_of_v<IComponent, C>;

typedef u32 ComponentTypeID;


template<Component C>
constexpr ComponentTypeID getComponentTypeID() {
	static_assert(false, "Component not registered");
	return 0;
}


#define COMPONENT_START_REGISTRY()	enum class ComponentType : ComponentTypeID {
#define REGISTER_COMPONENT_CLASS(c) c,
#define COMPONENT_END_REGISTRY() 	};

#define GENERATE_COMPONENT_ACCESSOR(c)						\
template<>													\
constexpr ComponentTypeID getComponentTypeID<c>() {			\
	return static_cast<ComponentTypeID>(ComponentType::c);	\
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