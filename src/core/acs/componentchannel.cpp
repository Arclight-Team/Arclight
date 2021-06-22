#include "componentchannel.h"




ComponentChannel::ComponentChannel() : provider(nullptr), actor(0) {}

void ComponentChannel::open(ComponentProvider& provider, ActorID actor) {
    this->provider = &provider;
    this->actor = actor;
}
