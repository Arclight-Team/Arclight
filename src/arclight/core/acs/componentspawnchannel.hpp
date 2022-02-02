/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 componentspawnchannel.hpp
 */

#pragma once

#include "actor.hpp"
#include "componentprovider.hpp"
#include "componentobserver.hpp"
#include "util/typetraits.hpp"



class ComponentSpawnChannel {

public:

	constexpr ComponentSpawnChannel(ComponentProvider& provider, ActorID actor, ComponentObserver& observer) noexcept : provider(provider), observer(observer), actor(actor) {}

	template<Component C>
	void add(C&& component) {
		
		if(provider.addComponent(actor, std::forward<C>(component))) {
			observer.record(ComponentEvent::Created, provider.getComponent<TT::RemoveConst<TT::RemoveRef<C>>>(actor), actor);
		}

	}

	template<Component C>
	void overwrite(C&& component) {

		provider.setComponent(actor, std::forward<C>(component));
		observer.record(ComponentEvent::Created, provider.getComponent<TT::RemoveConst<TT::RemoveRef<C>>>(actor), actor);

	}

	template<Component C>
	bool contains() const {
		return provider.hasComponent<C>(actor);
	}

	constexpr ActorID getActor() const noexcept {
		return actor;
	}

private:

	ComponentProvider& provider;
	ComponentObserver& observer;
	ActorID actor;

};