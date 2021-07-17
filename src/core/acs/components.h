#pragma once

#include "component/transform.h"
#include "component/boxcollider.h"

#include <tuple>


using ComponentTypes = std::tuple<Transform, BoxCollider>;


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
    using SharedType = std::decay_t<C>;

};