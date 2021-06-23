#pragma once

#include "componentchannel.h"
#include "componentprovider.h"

#include <unordered_map>
#include <functional>
#include <memory>



class Transform;

class ActorManager {

public:

    typedef std::function<void(ComponentChannel&)> ConstructionFunction;

    ActorManager();

    void setup();

    template<ActorBlueprint Actor>
    void registerActor(ActorTypeID id) {
        registerActor(id, std::make_unique<Actor>());
    }

    void registerActor(ActorTypeID id, std::unique_ptr<IActor> blueprint);

    ActorID spawn(ActorTypeID id);
    ActorID spawn(ActorTypeID id, const Transform& transform);
    ActorID spawn(ActorTypeID id, const ConstructionFunction& onConstruct);

private:

    bool isActorTypeRegistered(ActorTypeID id);
    ActorID getNextActorID();

    ComponentProvider provider;
    std::unordered_map<ActorTypeID, std::unique_ptr<IActor>> registeredActorTypes;

};