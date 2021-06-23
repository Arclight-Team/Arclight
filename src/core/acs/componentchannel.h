#pragma once

#include "actor/actor.h"
#include "componentprovider.h"


class ComponentChannel {

public:

    ComponentChannel();
    ComponentChannel(ComponentProvider& provider, ActorID actor);

    void open(ComponentProvider& provider, ActorID actor);

    template<Component C>
    void add(C&& component) {
        provider->addComponent(actor, std::forward<C>(component));
    }

    template<Component C>
    void overwrite(C&& component) {
        provider->setComponent(actor, std::forward<C>(component));
    }

private:

    ComponentProvider* provider;
    ActorID actor;

};
