#pragma once

#include "actor.h"


/*
    Example actor
*/
class ExampleActor : public IActor {

public:

    virtual void onCreate(ComponentChannel& channel) override;

};