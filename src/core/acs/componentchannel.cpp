#include "componentchannel.h"



ComponentChannel::ComponentChannel() : provider(nullptr), actor(0) {}

ComponentChannel::ComponentChannel(ComponentProvider& provider, ActorID actor) {
    open(provider, actor);
}


void ComponentChannel::open(ComponentProvider& provider, ActorID actor) {
    this->provider = &provider;
    this->actor = actor;
}
