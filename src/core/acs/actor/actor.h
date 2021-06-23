#pragma once

#include "../component/component.h"
#include "util/assert.h"
#include "types.h"


typedef u64 ActorID;
typedef u32 ActorTypeID;

class ComponentChannel;

/*
    Actor interface for all blueprints
*/
class IActor {

public:

    virtual void onCreate(ComponentChannel& channel) {
        __pure_construct(channel);
    }

private:

    static void __pure_construct(ComponentChannel&) {
        arc_force_assert("Illegal invokation of actor constructor");
    }

};


template<class A>
concept ActorBlueprint = BaseOf<IActor, std::remove_reference_t<A>>;
