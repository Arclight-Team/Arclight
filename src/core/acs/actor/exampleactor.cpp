#include "exampleactor.h"
#include "../componentchannel.h"
#include "../component/transform.h"



void ExampleActor::construct(ComponentChannel& channel) {

    channel.add(Transform{});

}