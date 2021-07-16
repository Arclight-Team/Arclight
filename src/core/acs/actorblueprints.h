#pragma once

#include "actor.h"



/*
    Example actor
*/
class ExampleActor : public IActor {

public:

    virtual void onCreate(ComponentSpawnChannel& channel) override;

};


/*
    Box actor
*/
class BoxActor : public IActor {

public:

    virtual void onCreate(ComponentSpawnChannel& channel) override;

};