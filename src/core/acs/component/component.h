#pragma once

#include "types.h"
#include "util/concepts.h"


class IComponent {};
typedef u32 ComponentTypeID;

template<class C>
concept Component = BaseOf<IComponent, std::remove_reference_t<C>>;


struct ComponentID {

	template<Component C>
	constexpr static ComponentTypeID getComponentTypeID() noexcept {
		static_assert(false, "Component not registered");
		return -1;
	}

	template<Component C>
	constexpr static ComponentTypeID get() noexcept {
		return getComponentTypeID<SharedType<C>>();
	}

	template<Component C>
	using SharedType = std::decay_t<C>;

};


#define REGISTER_COMPONENT(c, id)											\
template<>																	\
constexpr ComponentTypeID ComponentID::getComponentTypeID<c>() noexcept{	\
	return id;																\
}
