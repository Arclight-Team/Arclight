/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 any.hpp
 */

#pragma once

#include "util/typetraits.hpp"
#include "util/concepts.hpp"
#include "types.hpp"

#include <typeinfo>
#include <new>


/*
    Exception thrown if the Any holds a different type
*/
class BadAnyAccess : public std::exception {

public:

    virtual const char* what() const noexcept override {
        return "Bad any access";
    }

};



/*
    Any is an object that allows storage of heterogeneous objects without
    specifying the possible types in advance.

    This class' interface was inspired by std::any as defined by ISO C++20
    On top of that, performance was accelerated in certain functions to allow zero access cost.

    Note that this implementation allows specifying custom buffer sizes/alignments
    to avoid dynamic allocations for performance-critical situations.
    As long as the type satisfies size/alignment constraints and has a noexcept move constructor, SBO is utilized.
*/
template<SizeT Size = PointerSize, AlignT Align = PointerAlign>
class Any {

    template<class>
    struct TypeTagged : public std::false_type {};
    
    template<class T>
    struct TypeTagged<TypeTag<T>> : public std::true_type {};

    template<class T>
    constexpr static bool TypeTaggedV = TypeTagged<T>::value;

public:

    /*
        Default constructor
        Constructs a new Any with no content
    */
    constexpr Any() noexcept : executor(nullptr) {}


    /*
        Copy constructor
        Copies the contents, if any
    */
    Any(const Any& other) {

        if(other.hasValue()) {

            Argument argument;
            argument.any = this;
            other.executor(&other, Operation::Copy, &argument);

        } else {
            executor = nullptr;
        }

    }


    /*
        Move constructor
        Moves the contents, if any
    */
    Any(Any&& other) noexcept {

        if(other.hasValue()) {

            Argument argument;
            argument.any = this;
            other.executor(&other, Operation::Move, &argument);

        } else {
            executor = nullptr;
        }

    }


    /*
        Value constructor
        Copy-constructs the object into the storage
    */
    template<class T>
    Any(T&& value) requires (!Equal<TT::Decay<T>, Any> && CopyConstructible<TT::Decay<T>> && !TypeTaggedV<T>) {

        using U = TT::Decay<T>;

        Executor<U>::construct(this, std::forward<T>(value));
        executor = &Executor<U>::execute;

    }


    /*
        In-place constructor
        In-place constructs the object into the storage
    */
    template<class T, class... Args>
    Any(TypeTag<T>, Args&&... args) requires Constructible<T, Args...> {
        
        using U = TT::Decay<T>;
        Executor<U>::construct(this, std::forward<Args>(args)...);
        executor = &Executor<U>::execute;

    }


    /*
        Destructor
        Destroys the storage
    */
    ~Any() noexcept {
        reset();
    }


    /*
        Copy-assignment operator
        Copy-constructs from the object's storage
    */
    Any& operator=(const Any& other) {

        if(!other.hasValue()) {
            
            reset();

        } else if (*this != other) {

            Argument argument;
            argument.any = this;
            other.executor(&other, Operation::Copy, &argument);

        }

        return *this;

    }


    /*
        Move-assignment operator
        Move-constructs from the object's storage
    */
    Any& operator=(Any&& other) noexcept {

        if(!other.hasValue()) {

            reset();

        } else if (*this != other) {

            //We must destruct the old object first
            reset();

            //Move it
            Argument argument;
            argument.any = this;
            other.executor(&other, Operation::Move, &argument);

        }

        return *this;

    }


    /*
        Value-assignment operator
        Copy-constructs new storage from the object
    */
    template<class T>
    Any& operator=(T&& value) requires CopyConstructible<TT::Decay<T>> {

        *this = Any(std::forward<T>(value));
        return *this;

    }


    /*
        Returns true if it contains an object, false otherwise
    */
    constexpr bool hasValue() const noexcept {
        return executor != nullptr;
    }


    /*
        Resets the storage and destroys the object if it exists
    */
    void reset() noexcept {

        if(hasValue()) {
            executor(this, Operation::Destruct, nullptr);
        }

    }


    /*
        In-place constructs an object of type T. Previous contents are destroyed.
    */
    template<class T, class... Args>
    void emplace(Args&&... args) requires Constructible<T, Args...> {

        using U = TT::Decay<T>;

        reset();

        Executor<U>::construct(this, std::forward<Args>(args)...);
        executor = &Executor<U>::execute;

    }


    /*
        Swaps the object's contents
    */
    void swap(Any& other) noexcept {

        if(!hasValue() && !other.hasValue()) {
            return;
        }

        if(hasValue() && other.hasValue()) {

            //Swap underlying contents
            Argument argument;
            Any temp;

            argument->any = &temp;
            other.executor(&other, Operation::Move, &argument);

            argument->any = &other;
            executor(this, Operation::Move, &argument);

            argument->any = this;
            temp.executor(&temp, Operation::Move, &argument);

        } else if (hasValue()) {

            //Transfer contents to other
            Argument argument;
            argument->any = &other;
            executor(this, Operation::Move, &argument);

        } else {

            //Receive contents from other
            Argument argument;
            argument->any = this;
            other.executor(&other, Operation::Move, &argument);

        }

    }


    /*
        Returns the std::type_info struct of the underlying object
    */
    const std::type_info& getTypeInfo() const noexcept {

        if(!hasValue()) {
            return typeid(void);
        }

        Argument argument;
        executor(this, Operation::TypeInfo, &argument);
        return *argument.type;

    }


    /*
        Casts the object to T.
        Throws BadAnyAccess if the conversion is illegal.
    */
    template<class T>
    const T& cast() const requires std::is_same_v<TT::RemoveCV<T>, TT::Decay<T>> {

        using U = TT::Decay<T>;

        if (!hasValue() || &Executor<U>::execute != executor) {
            throw BadAnyAccess();
        } else {
            return Executor<U>::get(this);
        }

    }


    /*
        Casts the object to T.
        If T is not the type of the underlying storage, behaviour is undefined.
    */
    template<class T>
    const T& unsafeCast() const noexcept requires std::is_same_v<TT::RemoveCV<T>, TT::Decay<T>> {
        return Executor<TT::Decay<T>>::get(this);
    }

    //See the const version of cast()
    template<class T>
    T& cast() requires std::is_same_v<TT::RemoveCV<T>, TT::Decay<T>> {
        return const_cast<T&>(static_cast<const Any*>(this)->cast<T>());
    }

    //See the const version of unsafeCast()
    template<class T>
    T& unsafeCast() noexcept requires std::is_same_v<TT::RemoveCV<T>, TT::Decay<T>> {
        return const_cast<T&>(static_cast<const Any*>(this)->unsafeCast<T>());
    }

private:

    enum class Operation {
        Destruct,
        Move,
        Copy,
        TypeInfo
    };

    union Argument {
        Any* any;
        const std::type_info* type;
    };

    typedef void(*StateExecutor)(const Any*, Operation, Argument*);

    alignas(Align) union Storage {

        constexpr Storage() : ptr(nullptr) {}

        constexpr Storage(const Storage& other) = delete;
        constexpr Storage& operator=(const Storage& other) = delete;

        void* ptr;
        u8 buffer[Size];

    } storage;

    StateExecutor executor;

    
    template<CopyConstructible T>
    struct Executor {

        //Condition: a) must fit into the buffer; b) alignment must be no stricter than those of buffer
        constexpr static bool StaticAllocatable = sizeof(T) <= Size && alignof(T) <= Align && std::is_nothrow_move_constructible_v<T>;

        template<class... Args>
        static void construct(Any* any, Args&&... args) requires Constructible<T, Args...> {

            if constexpr (StaticAllocatable) {
                ::new(any->storage.buffer) T(std::forward<Args>(args)...);
            } else {
                any->storage.ptr = new T(std::forward<Args>(args)...);
            }

        }

        static const T& get(const Any* any) noexcept {
            
            if constexpr (StaticAllocatable) {
                return *reinterpret_cast<const T*>(any->storage.buffer);
            } else {
                return *static_cast<const T*>(any->storage.ptr);
            }

        }

        static void execute(const Any* any, Operation operation, Argument* arg) {

            switch(operation) {

                case Operation::Destruct:
                    {
                        Any* a = const_cast<Any*>(any);

                        if constexpr (StaticAllocatable) {
                            reinterpret_cast<T*>(a->storage.buffer)->~T();
                        } else {
                            delete static_cast<T*>(a->storage.ptr);
                        }

                        a->executor = nullptr;
                    }
                    break;

                case Operation::Move:

                    if constexpr (StaticAllocatable) {

                        Any* from = const_cast<Any*>(any);
                        Any* to = arg->any;
                        T* ptr = reinterpret_cast<T*>(from->storage.buffer);

                        ::new(to->storage.buffer) T(std::move(*ptr));
                        ptr->~T();

                        to->executor = from->executor;
                        from->executor = nullptr;

                    } else {

                        Any* from = const_cast<Any*>(any);
                        Any* to = arg->any;

                        to->storage.ptr = from->storage.ptr;
                        to->executor = from->executor;
                        from->executor = nullptr;

                    }

                    break;

                case Operation::Copy:

                    if constexpr (StaticAllocatable) {

                        const Any* from = any;
                        Any* to = arg->any;
                        const T* ptr = reinterpret_cast<const T*>(from->storage.buffer);

                        ::new(to->storage.buffer) T(*ptr);
                        to->executor = from->executor;

                    } else {

                        const Any* from = any;
                        Any* to = arg->any;
                        T* ptr = static_cast<T*>(from->storage.ptr);

                        to->storage.ptr = new T(*ptr);
                        to->executor = from->executor;

                    }

                    break;

                case Operation::TypeInfo:
                    arg->type = &typeid(T);
                    break;

            }

        }

    };

};



/*
    NocopyAny is a variant of Any that may also contain non-copyable types.
    Copy semantics have therefore been deleted.
*/
template<SizeT Size = PointerSize, AlignT Align = PointerAlign>
class NocopyAny {

    template<class>
    struct TypeTagged : public std::false_type {};
    
    template<class T>
    struct TypeTagged<TypeTag<T>> : public std::true_type {};

    template<class T>
    constexpr static bool TypeTaggedV = TypeTagged<T>::value;

public:

    /*
        Default constructor
        Constructs a new NocopyAny with no content
    */
    constexpr NocopyAny() noexcept : executor(nullptr) {}


    /*
        Copy constructor & copy assignment operator
        [Deleted]
    */
    NocopyAny(NocopyAny& any) noexcept = delete;
    NocopyAny& operator=(const NocopyAny& any) noexcept = delete;


    /*
        Move constructor
        Moves the contents, if any
    */
    NocopyAny(NocopyAny&& other) noexcept {

        if(other.hasValue()) {

            Argument argument;
            argument.any = this;
            other.executor(&other, Operation::Move, &argument);

        } else {
            executor = nullptr;
        }

    }


    /*
        Value constructor
        Copy-constructs the object into the storage
    */
    template<class T>
    NocopyAny(T&& value) requires (!Equal<TT::Decay<T>, NocopyAny> && MoveConstructible<TT::Decay<T>> && !TypeTaggedV<T>) {

        using U = TT::Decay<T>;

        Executor<U>::construct(this, std::forward<T>(value));
        executor = &Executor<U>::execute;

    }


    /*
        In-place constructor
        In-place constructs the object into the storage
    */
    template<class T, class... Args>
    NocopyAny(TypeTag<T>, Args&&... args) requires Constructible<T, Args...> {
        
        using U = TT::Decay<T>;
        Executor<U>::construct(this, std::forward<Args>(args)...);
        executor = &Executor<U>::execute;

    }


    /*
        Destructor
        Destroys the storage
    */
    ~NocopyAny() noexcept {
        reset();
    }



    /*
        Move-assignment operator
        Move-constructs from the object's storage
    */
    NocopyAny& operator=(NocopyAny&& other) noexcept {

        if(!other.hasValue()) {

            reset();

        } else if (*this != other) {

            //We must destruct the old object first
            reset();

            //Move it
            Argument argument;
            argument.any = this;
            other.executor(&other, Operation::Move, &argument);

        }

        return *this;

    }


    /*
        Value-assignment operator
        Copy-constructs new storage from the object
    */
    template<class T>
    NocopyAny& operator=(T&& value) requires MoveConstructible<TT::Decay<T>> {

        *this = NocopyAny(std::forward<T>(value));
        return *this;

    }


    /*
        Returns true if it contains an object, false otherwise
    */
    constexpr bool hasValue() const noexcept {
        return executor != nullptr;
    }


    /*
        Resets the storage and destroys the object if it exists
    */
    void reset() noexcept {

        if(hasValue()) {
            executor(this, Operation::Destruct, nullptr);
        }

    }


    /*
        In-place constructs an object of type T. Previous contents are destroyed.
    */
    template<class T, class... Args>
    void emplace(Args&&... args) requires Constructible<T, Args...> {

        using U = TT::Decay<T>;

        reset();

        Executor<U>::construct(this, std::forward<Args>(args)...);
        executor = &Executor<U>::execute;

    }


    /*
        Swaps the object's contents
    */
    void swap(NocopyAny& other) noexcept {

        if(!hasValue() && !other.hasValue()) {
            return;
        }

        if(hasValue() && other.hasValue()) {

            //Swap underlying contents
            Argument argument;
            NocopyAny temp;

            argument->any = &temp;
            other.executor(&other, Operation::Move, &argument);

            argument->any = &other;
            executor(this, Operation::Move, &argument);

            argument->any = this;
            temp.executor(&temp, Operation::Move, &argument);

        } else if (hasValue()) {

            //Transfer contents to other
            Argument argument;
            argument->any = &other;
            executor(this, Operation::Move, &argument);

        } else {

            //Receive contents from other
            Argument argument;
            argument->any = this;
            other.executor(&other, Operation::Move, &argument);

        }

    }


    /*
        Returns the std::type_info struct of the underlying object
    */
    const std::type_info& getTypeInfo() const noexcept {

        if(!hasValue()) {
            return typeid(void);
        }

        Argument argument;
        executor(this, Operation::TypeInfo, &argument);
        return *argument.type;

    }


    /*
        Casts the object to T.
        Throws BadAnyAccess if the conversion is illegal.
    */
    template<class T>
    const T& cast() const requires std::is_same_v<TT::RemoveCV<T>, TT::Decay<T>> {

        using U = TT::Decay<T>;

        if (!hasValue() || &Executor<U>::execute != executor) {
            throw BadAnyAccess();
        } else {
            return Executor<U>::get(this);
        }

    }


    /*
        Casts the object to T.
        If T is not the type of the underlying storage, behaviour is undefined.
    */
    template<class T>
    const T& unsafeCast() const noexcept requires std::is_same_v<TT::RemoveCV<T>, TT::Decay<T>> {
        return Executor<TT::Decay<T>>::get(this);
    }

    //See the const version of cast()
    template<class T>
    T& cast() requires std::is_same_v<TT::RemoveCV<T>, TT::Decay<T>> {
        return const_cast<T&>(static_cast<const NocopyAny*>(this)->cast<T>());
    }

    //See the const version of unsafeCast()
    template<class T>
    T& unsafeCast() noexcept requires std::is_same_v<TT::RemoveCV<T>, TT::Decay<T>> {
        return const_cast<T&>(static_cast<const NocopyAny*>(this)->unsafeCast<T>());
    }

private:

    enum class Operation {
        Destruct,
        Move,
        TypeInfo
    };

    union Argument {
        NocopyAny* any;
        const std::type_info* type;
    };

    typedef void(*StateExecutor)(const NocopyAny*, Operation, Argument*);

    alignas(Align) union Storage {

        constexpr Storage() : ptr(nullptr) {}

        constexpr Storage(const Storage& other) = delete;
        constexpr Storage& operator=(const Storage& other) = delete;

        void* ptr;
        u8 buffer[Size];

    } storage;

    StateExecutor executor;

    
    template<MoveConstructible T>
    struct Executor {

        //Condition: a) must fit into the buffer; b) alignment must be no stricter than those of buffer
        constexpr static bool StaticAllocatable = sizeof(T) <= Size && alignof(T) <= Align && std::is_nothrow_move_constructible_v<T>;

        template<class... Args>
        static void construct(NocopyAny* any, Args&&... args) requires Constructible<T, Args...> {

            if constexpr (StaticAllocatable) {
                ::new(any->storage.buffer) T(std::forward<Args>(args)...);
            } else {
                any->storage.ptr = new T(std::forward<Args>(args)...);
            }

        }

        static const T& get(const NocopyAny* any) noexcept {
            
            if constexpr (StaticAllocatable) {
                return *reinterpret_cast<const T*>(any->storage.buffer);
            } else {
                return *static_cast<const T*>(any->storage.ptr);
            }

        }

        static void execute(const NocopyAny* any, Operation operation, Argument* arg) {

            switch(operation) {

                case Operation::Destruct:
                    {
                        NocopyAny* a = const_cast<NocopyAny*>(any);

                        if constexpr (StaticAllocatable) {
                            reinterpret_cast<T*>(a->storage.buffer)->~T();
                        } else {
                            delete static_cast<T*>(a->storage.ptr);
                        }

                        a->executor = nullptr;
                    }
                    break;

                case Operation::Move:

                    if constexpr (StaticAllocatable) {

                        NocopyAny* from = const_cast<NocopyAny*>(any);
                        NocopyAny* to = arg->any;
                        T* ptr = reinterpret_cast<T*>(from->storage.buffer);

                        ::new(to->storage.buffer) T(std::move(*ptr));
                        ptr->~T();

                        to->executor = from->executor;
                        from->executor = nullptr;

                    } else {

                        NocopyAny* from = const_cast<NocopyAny*>(any);
                        NocopyAny* to = arg->any;

                        to->storage.ptr = from->storage.ptr;
                        to->executor = from->executor;
                        from->executor = nullptr;

                    }

                    break;

                case Operation::TypeInfo:
                    arg->type = &typeid(T);
                    break;

            }

        }

    };

};



/*
    Helper template for optimized Any's
*/
template<class T>
using FastAny = Any<sizeof(T), alignof(T)>;

template<class T>
using FastNocopyAny = NocopyAny<sizeof(T), alignof(T)>;