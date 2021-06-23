#include "actormanager.h"
#include "component/transform.h"


ActorManager::ActorManager() {}


void ActorManager::setup() {

    provider.createArray<Transform>();

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

    ComponentChannel channel(provider, actorID);
    registeredActorTypes[id]->onCreate(channel);

    return actorID;

}



ActorID ActorManager::spawn(ActorTypeID id, const Transform& transform) {

    arc_assert(isActorTypeRegistered(id), "Cannot spawn unknown actor %d", id);

    ActorID actorID = getNextActorID();

    ComponentChannel channel(provider, actorID);
    channel.add(transform);
    registeredActorTypes[id]->onCreate(channel);

    return actorID;

}




ActorID ActorManager::spawn(ActorTypeID id, const ConstructionFunction& onConstruct) {

    arc_assert(isActorTypeRegistered(id), "Cannot spawn unknown actor %d", id);

    ActorID actorID = getNextActorID();

    ComponentChannel channel(provider, actorID);

    if(onConstruct) {
        onConstruct(channel);
    }
    
    registeredActorTypes[id]->onCreate(channel);

    return actorID;

}



bool ActorManager::isActorTypeRegistered(ActorTypeID id) {
    return registeredActorTypes.contains(id);
}



//Uh yea this will change in the future
ActorID ActorManager::getNextActorID() {
    static ActorID id = 0;
    return id++;
}
