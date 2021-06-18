#pragma once

#include "util/sparsearray.h"
#include "util/any.h"
#include "component.h"
#include "arcconfig.h"



class ComponentProvider {

    typedef Any<sizeof(SparseArray<void*>), alignof(SparseArray<void*>)> ComponentArray;

public:

    constexpr ComponentProvider() {
        componentArrays.reserve(ARC_ACS_MAX_COMPONENTS);
    }

    template<Component C>
    void createArray() {

        ComponentTypeID id = getComponentTypeID<C>();

        if(id >= componentArrays.size()) {

            if(id >= ARC_ACS_MAX_COMPONENTS) {
                Log::error("ACS", "ID %d exceeds the maximum component ID of %d", id, ARC_ACS_MAX_COMPONENTS - 1);
                return;
            }

            componentArrays.resize(id + 1);
            ComponentArray& array = componentArrays.back();
            array.emplace<SparseArray<C>>();

        }

    }

    template<Component C>
    SparseArray<C>& getComponentArray() {
        return componentCast<C>(getComponentTypeID<C>());
    }


private:

    template<Component C>
    ComponentTypeID getComponentTypeID() const noexcept {

        ComponentTypeID id = getComponentTypeID<C>();
        arc_assert(id < componentArrays.size(), "Component ID %d out of bounds", id);

        return id;

    }

    template<Component C>
    SparseArray<C>& componentCast(ComponentTypeID id) {
#ifdef ARC_ACS_RUNTIME_CHECKS
        return componentArrays[id].cast<C>();
#else
        return componentArrays[id].unsafeCast<C>();
#endif
    }

    std::vector<ComponentArray> componentArrays;

};