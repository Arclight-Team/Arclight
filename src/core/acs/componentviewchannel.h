#pragma once

#include "actor.h"
#include "componentprovider.h"



class ComponentViewChannel {

public:

    constexpr ComponentViewChannel(const ComponentProvider& provider, ActorID actor) noexcept : provider(provider), actor(actor) {}

    constexpr void shift(ActorID actor) noexcept {
        this->actor = actor;
    }

    template<Component C>
    bool contains() const {
        return provider.hasComponent<C>(actor);
    }

    constexpr ActorID getActor() const noexcept {
        return actor;
    }


private:

    const ComponentProvider& provider;
    ActorID actor;

};