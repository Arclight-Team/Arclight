#pragma once

#include "component/component.h"
#include "util/sparsearray.h"
#include "util/any.h"
#include "arcconfig.h"
#include "actor.h"



template<class T>
using ComponentArray = SparseArray<T, ActorID>;


class ComponentProvider {

    using ComponentArrayStorage = FastAny<ComponentArray<void*>>;

public:

    constexpr ComponentProvider() {
        componentArrays.reserve(ARC_ACS_MAX_COMPONENTS);
    }

    template<Component C>
    void createArray() {

        ComponentTypeID id = ComponentID::get<C>();

        if(id >= componentArrays.size()) {

            if(id >= ARC_ACS_MAX_COMPONENTS) {
                Log::error("ACS", "ID %d exceeds the maximum component ID of %d", id, ARC_ACS_MAX_COMPONENTS - 1);
                return;
            }

            componentArrays.resize(id + 1);
            componentArrays[id].emplace<ComponentArray<C>>();

        }

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
    SizeT getActorCount() const {
        return getComponentArray<C>().getSize();
    }

private:

    template<Component... Types>
    friend class ComponentView;

    template<Component C>
    auto& getComponentArray() {
        return componentCast<C>(ComponentID::get<C>());
    }

    template<Component C>
    const auto& getComponentArray() const {
        return componentCast<C>(ComponentID::get<C>());
    }

    template<Component C>
    auto& componentCast(ComponentTypeID id) {
#ifdef ARC_ACS_RUNTIME_CHECKS
        return componentArrays[id].cast<ComponentArray<ComponentID::SharedType<C>>>();
#else
        return componentArrays[id].unsafeCast<ComponentArray<C>>();
#endif
    }

    template<Component C>
    const auto& componentCast(ComponentTypeID id) const {
#ifdef ARC_ACS_RUNTIME_CHECKS
        return componentArrays[id].cast<ComponentArray<ComponentID::SharedType<C>>>();
#else
        return componentArrays[id].unsafeCast<ComponentArray<C>>();
#endif
    }

    std::vector<ComponentArrayStorage> componentArrays;

};