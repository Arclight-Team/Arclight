#include "actormanager.h"

#include "component/transform.h"
#include "component/boxcollider.h"


ActorManager::ActorManager() {}


void ActorManager::setup() {

    provider.createArray<Transform>();
    provider.createArray<BoxCollider>();

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



bool ActorManager::isActorTypeRegistered(ActorTypeID id) {
    return registeredActorTypes.contains(id);
}



//Uh yea this will change in the future
ActorID ActorManager::getNextActorID() {
    static ActorID id = 0;
    return id++;
}
