#pragma once

#include "componentchannel.h"
#include "componentprovider.h"
#include <unordered_map>



class ActorManager {

public:

    ActorManager();

    template<class... Args>
    ActorID spawn(ActorTypeID id, Args&&... args) {

        ActorID actorID = getNextActorID();

        ComponentChannel channel;
        channel.open(provider, actorID);
        //registeredActorTypes[id].construct(channel);

        return actorID;

    }

private:

    ActorID getNextActorID();

    ComponentProvider provider;
    std::unordered_map<ActorTypeID, IActor*> registeredActorTypes;

};