#pragma once

#include "../component/component.h"
#include "types.h"


typedef u64 ActorID;
typedef u32 ActorTypeID;

class ComponentChannel;

/*
    Base class for all actors
*/
class IActor {

public:

    IActor();

    virtual void construct(ComponentChannel& channel) = 0;

};
