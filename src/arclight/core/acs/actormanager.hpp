/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 actormanager.hpp
 */

#pragma once

#include "componentspawnchannel.hpp"
#include "componentprovider.hpp"
#include "componentobserver.hpp"
#include "componentview.hpp"

#include <unordered_map>
#include <functional>
#include <memory>



class Transform;

class ActorManager {

public:

	typedef std::function<void(ComponentSpawnChannel&)> ConstructionFunction;

	ActorManager();

	void setup();

	template<ActorBlueprint Actor>
	void registerActor(ActorTypeID id) {
		registerActor(id, std::make_unique<Actor>());
	}

	void registerActor(ActorTypeID id, std::unique_ptr<IActor> blueprint);

	ActorID spawn(ActorTypeID id);
	ActorID spawn(ActorTypeID id, const Transform& transform);
	ActorID spawn(ActorTypeID id, const ConstructionFunction& onConstruct);

	void destroy(ActorID actor);

	template<Component C, class Func>
	void addObserver(ComponentEvent event, Func&& callback) {
		observer.observe<C>(event, std::forward<Func>(callback));
	}

	template<Component... Types>
	ComponentView<Types...> view() {
		return ComponentView<Types...>(provider);
	}

	ComponentProvider& getProvider();
	const ComponentProvider& getProvider() const;

private:

	bool isActorTypeRegistered(ActorTypeID id);
	ActorID getNextActorID();

	ComponentProvider provider;
	ComponentObserver observer;
	std::unordered_map<ActorTypeID, std::unique_ptr<IActor>> registeredActorTypes;

};