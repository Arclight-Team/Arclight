#pragma once

#include "stdext/sparsearray.h"
#include "stdext/any.h"
#include "components.h"
#include "arcconfig.h"
#include "actor.h"



template<class T>
using ComponentArray = SparseArray<T, ActorID>;


class ComponentProvider {

	using ComponentArrayStorage = FastNocopyAny<ComponentArray<void*>>;

public:

	constexpr ComponentProvider() {
		componentArrays.reserve(ARC_ACS_MAX_COMPONENTS);
	}

	template<Component C>
	void createArray() {

		ComponentID id = ComponentHelper::getComponentID<C>();

		if (id >= componentArrays.size()) {

			if (id >= ARC_ACS_MAX_COMPONENTS) {
				Log::error("ACS", "ID %d exceeds the maximum component ID of %d", id, ARC_ACS_MAX_COMPONENTS - 1);
				return;
			}

			componentArrays.resize(id + 1);

		}

		componentArrays[id].emplace<ComponentArray<C>>();

	}

	template<Component C>
	bool addComponent(ActorID actor, C&& component) {
		return getComponentArray<C>().add(actor, std::forward<C>(component));
	}

	template<Component C>
	void setComponent(ActorID actor, C&& component) {
		getComponentArray<C>().set(actor, std::forward<C>(component));
	}

	template<Component C>
	C& getComponent(ActorID id) {
		return getComponentArray<C>()[id & 0xFFFFFFFF];
	}

	template<Component C>
	const C& getComponent(ActorID id) const {
		return getComponentArray<C>()[id & 0xFFFFFFFF];
	}

	template<Component C>
	OptionalRef<C> tryGetComponent(ActorID id) {
		return getComponentArray<C>().tryGet(id & 0xFFFFFFFF);
	}

	template<Component C>
	OptionalRef<const C> tryGetComponent(ActorID id) const {
		return getComponentArray<C>().tryGet(id & 0xFFFFFFFF);
	}

	template<Component C>
	bool hasComponent(ActorID id) const {
		return getComponentArray<C>().contains(id & 0xFFFFFFFF);
	}

	template<Component C>
	bool tryRemoveComponent(ActorID id) {
		return getComponentArray<C>().tryRemove(id & 0xFFFFFFFF);
	}

	template<Component C>
	void removeComponent(ActorID id) {
		getComponentArray<C>().remove(id & 0xFFFFFFFF);
	}

	template<Component C>
	SizeT getActorCount() const {
		return getComponentArray<C>().getSize();
	}

private:

	template<Component... Types>
	friend class ComponentView;

	template<Component C>
	auto& getComponentArray() {
		return componentCast<C>(ComponentHelper::getComponentID<C>());
	}

	template<Component C>
	const auto& getComponentArray() const {
		return componentCast<C>(ComponentHelper::getComponentID<C>());
	}

	template<Component C>
	auto& componentCast(ComponentID id) {
#ifdef ARC_ACS_RUNTIME_CHECKS
		return componentArrays[id].cast<ComponentArray<ComponentHelper::SharedType<C>>>();
#else
		return componentArrays[id].unsafeCast<ComponentArray<ComponentHelper::SharedType<C>>>();
#endif
	}

	template<Component C>
	const auto& componentCast(ComponentID id) const {
#ifdef ARC_ACS_RUNTIME_CHECKS
		return componentArrays[id].cast<ComponentArray<ComponentHelper::SharedType<C>>>();
#else
		return componentArrays[id].unsafeCast<ComponentArray<ComponentHelper::SharedType<C>>>();
#endif
	}

	std::vector<ComponentArrayStorage> componentArrays;

};