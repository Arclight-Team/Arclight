#include "actormanager.h"



ActorManager::ActorManager() {}


//Uh yea this will change in the future
ActorID ActorManager::getNextActorID() {
    static ActorID id = 0;
    return id++;
}
