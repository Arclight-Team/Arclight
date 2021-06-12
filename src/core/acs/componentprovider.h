#pragma once

#include "util/sparsearray.h"
#include "component.h"



class TransformComponent;

class ComponentStorage {

public:



private:
    //SparseArray<TransformComponent> k;

};



class ComponentProvider {

public:

    void create();

    template<Component C>
    SparseArray<C>& getComponentArray() {

        ComponentTypeID id = getComponentTypeID<C>();
        return componentCast<C>(id);

    }


    auto getComponentArray(ComponentTypeID id) {

    }


private:

    template<Component C>
    SparseArray<C>& componentCast(ComponentTypeID id) {
        return static_cast<SparseArray<C>>(components[id]);
    }

    ComponentStorage components;


};