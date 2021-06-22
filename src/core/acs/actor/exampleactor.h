#pragma once

#include "actor.h"


/*
    Example actor
*/
class ExampleActor : public IActor {

public:

    virtual void construct(ComponentChannel& channel) override;

};