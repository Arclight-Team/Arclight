/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 actor.hpp
 */

#pragma once

#include "util/assert.hpp"
#include "util/typetraits.hpp"
#include "types.hpp"


typedef u64 ActorID;
typedef u32 ActorTypeID;

class ComponentSpawnChannel;

/*
    Actor interface for all blueprints
*/
class IActor {

public:

    virtual void onCreate(ComponentSpawnChannel& channel) {
        __pure_construct(channel);
    }

private:

    static void __pure_construct(ComponentSpawnChannel&) {
        arc_force_assert("Illegal invokation of actor constructor");
    }

};


template<class A>
concept ActorBlueprint = BaseOf<IActor, TT::RemoveRef<A>>;
