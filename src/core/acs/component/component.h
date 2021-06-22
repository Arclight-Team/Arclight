#pragma once

#include "types.h"
#include "util/concepts.h"


class IComponent {};

typedef u32 ComponentTypeID;

template<class C>
concept Component = BaseOf<IComponent, C>;


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