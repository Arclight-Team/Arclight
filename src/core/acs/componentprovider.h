#pragma once

#include "util/sparsearray.h"
#include "util/any.h"
#include "component/component.h"
#include "arcconfig.h"
#include "actor/actor.h"



class ComponentProvider {

    typedef Any<sizeof(SparseArray<void*>), alignof(SparseArray<void*>)> ComponentArrayStorage;
    
    template<class T>
    using ComponentArray = SparseArray<T, ActorID>;

public:

    constexpr ComponentProvider() {
        componentArrays.reserve(ARC_ACS_MAX_COMPONENTS);
    }

    template<Component C>
    void createArray() {

        ComponentTypeID id = getComponentTypeID<C>();

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
    void addComponent(ActorID actor, C&& component) {
        getComponentArray<C>().add(actor, std::forward<C>(component));
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


private:

    template<Component C>
    auto& getComponentArray() {

        using CX = std::decay_t<C>;
        return componentCast<CX>(getComponentTypeID<CX>());

    }

    template<Component C>
    ComponentTypeID getComponentTypeID() const noexcept {

        using CX = std::decay_t<C>;
        ComponentTypeID id = ComponentID::getComponentTypeID<CX>();

        return id;

    }

    template<Component C>
    auto& componentCast(ComponentTypeID id) {
#ifdef ARC_ACS_RUNTIME_CHECKS
        return componentArrays[id].cast<ComponentArray<C>>();
#else
        return componentArrays[id].unsafeCast<ComponentArray<C>>();
#endif
    }

    std::vector<ComponentArrayStorage> componentArrays;

};