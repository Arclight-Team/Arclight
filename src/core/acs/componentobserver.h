#pragma once

#include "components.h"
#include "actor.h"
#include "arcconfig.h"
#include "util/log.h"
#include "util/any.h"
#include "util/concepts.h"

#include <vector>
#include <functional>


enum class ComponentEvent {
    Created,
    Destroyed
};


class ComponentObserver {

    using NullFunctionType = std::function<void()>;
    using AnyCallback = FastAny<NullFunctionType>;

public:

    constexpr static u32 eventCount = 2;

    template<Component C>
    using Function = std::function<void(ComponentHelper::SharedType<C>&, ActorID)>;

    ComponentObserver() {
        observerInvokables.reserve(getObserverEntryIndex(ARC_ACS_MAX_COMPONENTS));
    }

    template<Component C, class Func> requires Constructible<Function<C>, Func&&>
    void observe(ComponentEvent event, Func&& callback) {

        constexpr ComponentID cid = ComponentHelper::getComponentID<C>();
        u32 oei = getObserverEntryIndex(cid, event);

        if(observerInvokables.size() <= oei) {

            if(cid >= ARC_ACS_MAX_COMPONENTS) {
                Log::error("ACS", "ID %d exceeds the maximum component ID of %d", cid, ARC_ACS_MAX_COMPONENTS - 1);
                return;
            }

            observerInvokables.resize(oei + 1);

        }

        auto& vec = observerInvokables[oei];
        vec.push_back(AnyCallback(TypeTag<Function<C>>{}, callback));

    }

    template<Component C>
    void clear() {

        constexpr ComponentID cid = ComponentHelper::get<C>();
        constexpr u32 oei = getObserverEntryIndex(cid);

        for(u32 i = 0; i < eventCount; i++) {

#ifdef ARC_ACS_RUNTIME_CHECKS
            if(observerInvokables.size() > (oei + i)) {
#endif
                observerInvokables[oei + i].clear();
#ifdef ARC_ACS_RUNTIME_CHECKS
            }
#endif

        }

    }

    void clearAll() {

        for(auto& vec : observerInvokables) {
            vec.clear();
        }

    }

    template<Component C>
    void invokeDirect(ComponentEvent event, C& component, ActorID actor) {

        constexpr ComponentID cid = ComponentHelper::getComponentID<C>();
        u32 oei = getObserverEntryIndex(cid, event);

        if(oei >= observerInvokables.size()) {
            return;
        }

        auto& vec = observerInvokables[oei];

        for(const auto& func : vec) {

#ifdef ARC_ACS_RUNTIME_CHECKS
            func.cast<Function<C>>()(component, actor);
#else
            func.unsafeCast<Function<C>>()(component, actor);
#endif

        }

    }

    template<Component C>
    void record(ComponentEvent event, C& component, ActorID actor) {

        constexpr ComponentID cid = ComponentHelper::getComponentID<C>();
        u32 oei = getObserverEntryIndex(cid, event);

        if(oei >= observerInvokables.size() || !observerInvokables[oei].size()) {
            return;
        }

        recordedInvokables[cid] = [=, &component, this]() {
            invokeDirect(event, component, actor);
        };

    }

    void invokeAll() {

        for(const auto&[cid, func] : recordedInvokables) {
            func();
        }

        recordedInvokables.clear();

    }


private:

    constexpr static u32 getObserverEntryIndex(ComponentID id, ComponentEvent event = static_cast<ComponentEvent>(0)) noexcept {
        return id * static_cast<u32>(eventCount) + static_cast<u32>(event);
    }

    std::vector<std::vector<AnyCallback>> observerInvokables;
    std::unordered_map<ComponentID, std::function<void()>> recordedInvokables;

};