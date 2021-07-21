#include "actorblueprints.h"
#include "componentspawnchannel.h"
#include "component/transform.h"
#include "component/rigidbody.h"



void ExampleActor::onCreate(ComponentSpawnChannel& channel) {

    channel.add(Transform{});

}



void BoxActor::onCreate(ComponentSpawnChannel& channel) {

    channel.add(Transform());
    channel.add(RigidBody());

}