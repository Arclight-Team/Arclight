#pragma once

#include "componentviewchannel.h"
#include "componentprovider.h"
#include "component/component.h"
#include "util/concepts.h"

#include <tuple>
#include <algorithm>



template<Component... Types>
class ComponentView {

    struct IteratorHelper {

        template<class T, bool Const>
        struct Container {

            using Iterator = std::conditional_t<Const, typename ComponentArray<T>::ConstIterator, typename ComponentArray<T>::Iterator>;

            constexpr Container(ComponentArray<T>& array, bool begin) : array(array), iterator([&]() { if constexpr (Const) { return begin ? array.cbegin() : array.cend(); } else { return begin ? array.begin() : array.end(); }}()) {}

            ComponentArray<T>& array;
            Iterator iterator;

        };

        using AnyContainer = FastAny<Container<void*, true>>;

        enum class Action {
            Reset,
            Increment,
            Decrement
        };

        template<class T, bool Const>
        static AnyContainer construct(ComponentProvider& provider, ComponentViewChannel& channel, bool begin) {

            AnyContainer container(TypeTag<Container<T, Const>>{}, provider.getComponentArray<T>(), begin);

            if(begin) {
                execute<T, Const>(container, channel, Action::Reset);   
            } else {
                channel.shift(-1);
            }

            return container;

        }

        template<class T, class... Pack>
        struct Exclude {
            using Tuple = decltype(std::tuple_cat(std::declval<std::conditional_t<std::is_same_v<T, Pack>, std::tuple<>, std::tuple<Pack>>>()...));
        };

        template<class>
        struct ApplyComposition;

        template<template<class...> class Tuple, class... Pack>
        struct ApplyComposition<Tuple<Pack...>> {

            static bool validateCompositionInternal(const ComponentViewChannel& channel) {
                return (channel.contains<Pack>() && ...);
            }

        };

        template<class T>
        static bool validateComposition(const ComponentViewChannel& channel) {
            return ApplyComposition<Exclude<T, Types...>::Tuple>::validateCompositionInternal(channel);
        }

        template<class T>
        constexpr static bool unequalEnd(const ComponentArray<T>& array, const typename ComponentArray<T>::ConstIterator& it) {
            return it != array.cend();
        }

        template<class T>
        constexpr static bool unequalEnd(const ComponentArray<T>& array, const typename ComponentArray<T>::Iterator& it) {
            return it != array.end();
        }


        template<class T, bool Const>
        static void execute(AnyContainer& containerHandle, ComponentViewChannel& channel, Action action) {

            Container<T, Const>& container = containerHandle.unsafeCast<Container<T, Const>>();
            auto& array = container.array;
            auto& it = container.iterator;

            switch(action) {

                case Action::Reset:
                    {
                        for(; unequalEnd(array, it); ++it) {

                            channel.shift(container.array.invert(it));

                            if(validateComposition<T>(channel)) {
                                return;
                            }

                        }

                        //Set to end
                        channel.shift(-1);
                    }
                    break;

                case Action::Increment:
                    {
                        while(true) {

                            ++it;

                            if(!unequalEnd(array, it)) {
                                channel.shift(-1);
                                break;
                            }

                            channel.shift(container.array.invert(it));

                            if(validateComposition<T>(channel)) {
                                break;
                            }

                        }
                    }
                    break;

                case Action::Decrement:
                    {            
                        while(true) {

                            --it;

                            channel.shift(container.array.invert(it));
                         
                            if(validateComposition<T>(channel)) {
                                break;
                            }

                        }
                    }
                    break;

                default:
                    arc_force_assert("Illegal case in component view iterator helper encountered");
                    break;

            }

        }

    };

    using IteratorConstructor = IteratorHelper::AnyContainer(*)(ComponentProvider&, ComponentViewChannel&, bool);
    using IteratorExecutor = void(*)(IteratorHelper::AnyContainer&, ComponentViewChannel&, IteratorHelper::Action);


public:

    static_assert((BaseType<Types> && ...), "View types must be non-qualified");

    constexpr static SizeT TypeCount = sizeof...(Types);


    template<bool Const>
    class IteratorBase {

    public:

        using Tuple             = std::tuple<std::conditional_t<Const, const Types&, Types&>...>;

        using difference_type   = std::ptrdiff_t;
        using value_type        = Tuple;
        using pointer           = value_type;
        using reference         = value_type;

        constexpr IteratorBase() noexcept : provider(nullptr), executor(nullptr) {}

        template<class = std::enable_if_t<Const, void>>
        constexpr IteratorBase(const IteratorBase<false>& it) noexcept : provider(it.provider), channel(it.channel), executor(it.executor), container(it.container) {}

        IteratorBase(ComponentProvider& provider, bool begin, IteratorConstructor invokableCtor, IteratorExecutor invokableExec) : provider(&provider), channel(provider, -1), executor(invokableExec), container([&]() { arc_assert(invokableCtor, "View iterator constructor cannot be null"); return invokableCtor(provider, channel, begin); } ()) {
            arc_assert(invokableExec, "View iterator executor cannot be null");
        }

        constexpr reference operator*() const noexcept { return std::tie(provider->getComponent<Types>(channel.getActor())...); }
        constexpr pointer operator->()  const noexcept { return std::tie(provider->getComponent<Types>(channel.getActor())...); }

        IteratorBase& operator++() {step(); return *this;}
        IteratorBase operator++(int) {IteratorBase cpy = *this; ++(*this); return cpy;}
        IteratorBase& operator--() {retreat(); return *this;}
        IteratorBase operator--(int) {IteratorBase cpy = *this; --(*this); return cpy;}
  
        constexpr auto operator<=>(const IteratorBase& other) const noexcept = default;

        constexpr auto operator==(const IteratorBase& other) const noexcept {
            return channel.getActor() == other.channel.getActor();
        }

    private:

        void step() {
            executor(container, channel, IteratorHelper::Action::Increment);
        }

        void retreat() {
            executor(container, channel, IteratorHelper::Action::Decrement);
        }

        ComponentProvider* provider;
        ComponentViewChannel channel;
        IteratorExecutor executor;
        IteratorHelper::AnyContainer container;

    };

    using Iterator = IteratorBase<false>;
    using ConstIterator = IteratorBase<true>;

    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;


    ComponentView(ComponentProvider& provider) : provider(provider) {

        SizeT sizes[TypeCount] = {provider.getActorCount<Types>()...};

        constexpr static IteratorConstructor itCtor[TypeCount] = {&IteratorHelper::template construct<Types, false>...};
        constexpr static IteratorConstructor constItCtor[TypeCount] = {&IteratorHelper::template construct<Types, true>...};
        constexpr static IteratorExecutor itExec[TypeCount] = {&IteratorHelper::template execute<Types, false>...};
        constexpr static IteratorExecutor constItExec[TypeCount] = {&IteratorHelper::template execute<Types, true>...};
        
        const auto min = std::min_element(std::begin(sizes), std::end(sizes));
        SizeT typeIndex = std::distance(std::begin(sizes), min);

        iterCtor = itCtor[typeIndex];
        iterExec = itExec[typeIndex];
        constIterCtor = constItCtor[typeIndex];
        constIterExec = constItExec[typeIndex];

    }
    

    /*
        Returns an iterator to the start of the view.
    */
    Iterator begin() {
        return Iterator(provider, true, iterCtor, iterExec);
    }


    ConstIterator begin() const {
        return cbegin();
    }


    /*
        Returns an iterator to the end of the view.
    */
    Iterator end() {
        return Iterator(provider, false, iterCtor, iterExec);
    }


    ConstIterator end() const {
        return cend();
    }


    
    /*
        Returns a const iterator to the start of the view.
    */
    ConstIterator cbegin() {
        return ConstIterator(provider, true, constIterCtor, constIterExec);
    }


    /*
        Returns a const iterator to the end of the view.
    */
    ConstIterator cend() {
        return ConstIterator(provider, false, constIterCtor, constIterExec);
    }


    /*
        Returns a reverse iterator to the start of the view.
    */
    ReverseIterator rbegin() {
        return ReverseIterator(end());
    }


    ConstReverseIterator rbegin() const {
        return ConstReverseIterator(end());
    }


    /*
        Returns a reverse iterator to the end of the view.
    */
    ReverseIterator rend() {
        return ReverseIterator(begin());
    }


    ConstReverseIterator rend() const {
        return ConstReverseIterator(begin());
    }


    /*
        Returns a const reverse iterator to the start of the view.
    */
    ConstReverseIterator crbegin() {
        return ConstReverseIterator(cend());
    }


    /*
        Returns a const reverse iterator to the end of the view.
    */
    ConstReverseIterator crend() {
        return ConstReverseIterator(cbegin());
    }


private:

    ComponentProvider& provider;
    IteratorConstructor iterCtor;
    IteratorConstructor constIterCtor;
    IteratorExecutor iterExec;
    IteratorExecutor constIterExec;

};