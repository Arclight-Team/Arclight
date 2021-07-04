#pragma once

#include "actor/actor.h"
#include "componentprovider.h"



class ComponentChannel {

public:

    constexpr ComponentChannel() noexcept : provider(nullptr), actor(0) {}

    constexpr ComponentChannel(ComponentProvider& provider, ActorID actor) noexcept {
        open(provider, actor);
    }

    constexpr void open(ComponentProvider& provider, ActorID actor) noexcept {
        this->provider = &provider;
        this->actor = actor;
    }

    constexpr void shift(ActorID actor) noexcept {
        this->actor = actor;
    }

    template<Component C>
    void add(C&& component) {
        provider->addComponent(actor, std::forward<C>(component));
    }

    template<Component C>
    void overwrite(C&& component) {
        provider->setComponent(actor, std::forward<C>(component));
    }

    template<Component C>
    bool contains() const {
        return provider->hasComponent<C>(actor);
    }

    constexpr ActorID getActor() const noexcept {
        return actor;
    }


private:

    ComponentProvider* provider;
    ActorID actor;

};
