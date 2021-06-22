#pragma once

#include "actor/actor.h"
#include "componentprovider.h"



class ComponentChannel {

public:

    ComponentChannel();

    void open(ComponentProvider& provider, ActorID actor);

    template<Component C>
    void add(C&& component) {
        provider->addComponent(actor, std::forward<C>(component));
    }

private:

    ComponentProvider* provider;
    ActorID actor;

};
