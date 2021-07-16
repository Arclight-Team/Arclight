#pragma once

#include "actor.h"
#include "componentprovider.h"
#include "componentobserver.h"



class ComponentSpawnChannel {

public:

    constexpr ComponentSpawnChannel(ComponentProvider& provider, ActorID actor, ComponentObserver& observer) noexcept : provider(provider), observer(observer), actor(actor) {}

    template<Component C>
    void add(C&& component) {
        
        if(provider.addComponent(actor, std::forward<C>(component))) {
            observer.record(provider.getComponent<std::remove_const_t<std::remove_reference_t<C>>>(actor), actor);
        }

    }

    template<Component C>
    void overwrite(C&& component) {

        provider.setComponent(actor, std::forward<C>(component));
        observer.record(provider.getComponent<std::remove_const_t<std::remove_reference_t<C>>>(actor), actor);

    }

    template<Component C>
    bool contains() const {
        return provider.hasComponent<C>(actor);
    }

    constexpr ActorID getActor() const noexcept {
        return actor;
    }

private:

    ComponentProvider& provider;
    ComponentObserver& observer;
    ActorID actor;

};