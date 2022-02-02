/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 components.hpp
 */

#pragma once

#include "component/component.hpp"
#include "util/typetraits.hpp"

#include <tuple>


//Add component types here
using ComponentTypes = std::tuple<>;


struct ComponentHelper {

	template<class, class>
	struct ComponentTypeHelper;

	template<Component C, template<Component...> class Tuple, Component... Pack>
	struct ComponentTypeHelper<C, Tuple<Pack...>> {

		constexpr static ComponentID getID() {

			static_assert((std::is_same_v<ComponentHelper::SharedType<C>, Pack> || ...), "Component not found");
			
			ComponentID id = 0;
			((std::is_same_v<ComponentHelper::SharedType<C>, Pack> ? false : ++id) && ...);

			return id;

		}

	};

	template<Component C>
	constexpr static ComponentID getComponentID() {
		return ComponentTypeHelper<SharedType<C>, ComponentTypes>::getID();
	}

	template<Component C>
	using SharedType = TT::Decay<C>;

};