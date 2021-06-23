#include "exampleactor.h"
#include "../componentchannel.h"
#include "../component/transform.h"



void ExampleActor::onCreate(ComponentChannel& channel) {

    channel.add(Transform{});

}