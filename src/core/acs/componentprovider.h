#pragma once

#include "util/sparsearray.h"
#include "component.h"
#include <vector>


class ComponentProvider {

public:

    void create();

    template<Component C>
    SparseArray<C>& getComponentArray() {

        ComponentTypeID id = getComponentTypeID<C>();
        return componentCast<C>(id);

    }


private:

    template<Component C>
    SparseArray<C>& componentCast(ComponentTypeID id) {
        return static_cast<SparseArray<C>>(components[id]);
    }

    std::vector<void*> components;

};