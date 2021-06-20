#pragma once

#include "component.h"
#include "types.h"


typedef u32 ActorTypeID;


class ActorBlueprint {

public:

    ActorBlueprint();

    template<Component C>
    void addComponent() {

    }

private:

    ActorTypeID id;
    std::vector<std::pair<ComponentID, IComponent*>> componentBlueprint;

};