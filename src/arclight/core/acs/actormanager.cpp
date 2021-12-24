#include "actormanager.hpp"
#include "components.hpp"



//Lifetime helper struct
template<class>
struct ComponentLifetimeHelper;

template<template<Component...> class Tuple, Component... Pack>
struct ComponentLifetimeHelper<Tuple<Pack...>> {

    static void createArrays(ComponentProvider& provider) {
        (provider.createArray<Pack>(), ...);
    }

    static void destroyActor(ComponentProvider& provider, ComponentObserver& observer, ActorID actor) {
        ((provider.hasComponent<Pack>(actor) ? (observer.invokeDirect<Pack>(ComponentEvent::Destroyed, provider.getComponent<Pack>(actor), actor), provider.removeComponent<Pack>(actor)) : void()), ...);
    }

};



ActorManager::ActorManager() {}


void ActorManager::setup() {
    ComponentLifetimeHelper<ComponentTypes>::createArrays(provider);
}



void ActorManager::registerActor(ActorTypeID id, std::unique_ptr<IActor> blueprint) {

    if(isActorTypeRegistered(id)) {
        Log::warn("Actor Manager", "Attempted to double-register an actor with ID = %d", id);
        return;
    }

    registeredActorTypes[id] = std::move(blueprint);

}



ActorID ActorManager::spawn(ActorTypeID id) {

    arc_assert(isActorTypeRegistered(id), "Cannot spawn unknown actor %d", id);

    ActorID actorID = getNextActorID();

    ComponentSpawnChannel channel(provider, actorID, observer);
    
    registeredActorTypes[id]->onCreate(channel);
    observer.invokeAll();

    return actorID;

}



ActorID ActorManager::spawn(ActorTypeID id, const Transform& transform) {

    arc_assert(isActorTypeRegistered(id), "Cannot spawn unknown actor %d", id);

    ActorID actorID = getNextActorID();

    ComponentSpawnChannel channel(provider, actorID, observer);
    channel.add(transform);

    registeredActorTypes[id]->onCreate(channel);
    observer.invokeAll();

    return actorID;

}




ActorID ActorManager::spawn(ActorTypeID id, const ConstructionFunction& onConstruct) {

    arc_assert(isActorTypeRegistered(id), "Cannot spawn unknown actor %d", id);

    ActorID actorID = getNextActorID();

    ComponentSpawnChannel channel(provider, actorID, observer);

    if(onConstruct) {
        onConstruct(channel);
    }
    
    registeredActorTypes[id]->onCreate(channel);
    observer.invokeAll();

    return actorID;

}



void ActorManager::destroy(ActorID actor) {
    ComponentLifetimeHelper<ComponentTypes>::destroyActor(provider, observer, actor);
}




ComponentProvider& ActorManager::getProvider() {
    return provider;
}



const ComponentProvider& ActorManager::getProvider() const {
    return provider;
}



bool ActorManager::isActorTypeRegistered(ActorTypeID id) {
    return registeredActorTypes.contains(id);
}



//Uh yea this will change in the future
ActorID ActorManager::getNextActorID() {
    static ActorID id = 0;
    return id++;
}
