#pragma once

#include "component/component.h"
#include "actor.h"
#include "arcconfig.h"
#include "util/log.h"
#include "util/any.h"
#include "util/concepts.h"

#include <vector>
#include <functional>



class ComponentObserver {

    using NullFunctionType = std::function<void()>;
    using AnyCallback = FastAny<NullFunctionType>;

public:

    template<Component C>
    using Function = std::function<void(ComponentID::SharedType<C>&, ActorID)>;

    ComponentObserver() {
        observerInvokables.reserve(ARC_ACS_MAX_COMPONENTS);
    }

    template<Component C, class Func> requires Constructible<Function<C>, Func&&>
    void observe(Func&& callback) {

        constexpr ComponentTypeID cid = ComponentID::get<C>();

        if(observerInvokables.size() <= cid) {

            if(cid >= ARC_ACS_MAX_COMPONENTS) {
                Log::error("ACS", "ID %d exceeds the maximum component ID of %d", cid, ARC_ACS_MAX_COMPONENTS - 1);
                return;
            }

            observerInvokables.resize(cid + 1);
            auto& vec = observerInvokables[cid];
            vec.push_back(AnyCallback(TypeTag<Function<C>>{}, callback));

        }

    }

    template<Component C>
    void clear() {

        constexpr ComponentTypeID cid = ComponentID::get<C>();

#ifdef ARC_ACS_RUNTIME_CHECKS
        if(observerInvokables.size() > cid) {
#endif
            observerInvokables[cid].clear();
#ifdef ARC_ACS_RUNTIME_CHECKS
        }
#endif

    }

    void clearAll() {

        for(auto& vec : observerInvokables) {
            vec.clear();
        }

    }

    template<Component C>
    void invokeDirect(C& component, ActorID actor) {

        constexpr ComponentTypeID cid = ComponentID::get<C>();

#ifdef ARC_ACS_RUNTIME_CHECKS
        if(cid >= observerInvokables.size()) {
            Log::error("ACS", "CID %d not registered for observer", cid);
            return;
        }
#endif

        auto& vec = observerInvokables[cid];

        for(const auto& func : vec) {

#ifdef ARC_ACS_RUNTIME_CHECKS
            func.cast<Function<C>>()(component, actor);
#else
            func.unsafeCast<Function<C>>()(component, actor);
#endif

        }

    }

    template<Component C>
    void record(C& component, ActorID actor) {

        constexpr ComponentTypeID cid = ComponentID::get<C>();

        if(cid >= observerInvokables.size() || !observerInvokables[cid].size()) {
            return;
        }

        recordedInvokables[cid] = [=, &component, this]() {
            invokeDirect(component, actor);
        };

    }

    void invokeAll() {

        for(const auto&[cid, func] : recordedInvokables) {
            func();
        }

        recordedInvokables.clear();

    }


private:

    std::vector<std::vector<AnyCallback>> observerInvokables;
    std::unordered_map<ComponentTypeID, std::function<void()>> recordedInvokables;

};
