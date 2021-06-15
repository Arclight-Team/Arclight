#pragma once

#include "types.h"
#include "concepts.h"
#include <typeinfo>
#include <new>


class BadAnyAccess : public std::exception {

public:

    virtual const char* what() const noexcept override {
        return "Bad any access";
    }

};



template<SizeT Size>
class Any {

    template<class>
    struct TypeTagged : public std::false_type {};
    
    template<class T>
    struct TypeTagged<TypeTag<T>> : public std::true_type {};

    template<class T>
    constexpr static bool TypeTaggedV = TypeTagged<T>::value;

public:

    constexpr Any() : executor(nullptr) {}

    constexpr Any(const Any& other) {

        if(other.hasValue()) {

            Argument argument;
            argument.any = this;
            other.executor(other, Operation::Copy, &argument);

        } else {
            executor = nullptr;
        }

    }

    constexpr Any(Any&& other) {

        if(other.hasValue()) {

            Argument argument;
            argument.any = this;
            other.executor(other, Operation::Move, &argument);

        } else {
            executor = nullptr;
        }

    }

    template<class T>
    constexpr Any(T&& value) requires CopyConstructible<std::decay_t<T>> && !TypeTaggedV<T> {

        using U = std::decay_t<T>;
        Executor<U>::construct(this, std::forward<T>(value));
        executor = &Executor<U>::execute;

    }


    template<class T, class... Args>
    constexpr Any(TypeTag<T>, Args&&... args) requires Constructible<T, Args...> {
        
        using U = std::decay_t<T>;
        Executor<U>::construct(this, std::forward<Args>(args)...);
        executor = &Executor<U>::execute;

    }


    ~Any() {
        reset();
    }


    constexpr Any& operator=(const Any& other) {

        if(!other.hasValue()) {
            
            reset();

        } else if (*this != other) {

            Argument argument;
            argument.any = this;
            other.executor(other, Operation::Copy, &argument);

        }

        return *this;

    }


    constexpr Any& operator=(Any&& other) {

        if(!other.hasValue()) {

            reset();

        } else if (*this != other) {

            //We must destruct the old object first
            reset();

            //Move it
            Argument argument;
            argument.any = this;
            other.executor(other, Operation::Move, &argument);

        }

        return *this;

    }


    template<class T>
    constexpr Any& operator=(T&& value) requires CopyConstructible<std::decay_t<T>> {

        *this = Any(std::forward<T>(value));
        return *this;

    }


    constexpr bool hasValue() const noexcept {
        return executor != nullptr;
    }


    void reset() {

        if(hasValue()) {
            executor(this, Operation::Destruct, nullptr);
        }

    }


    template<class T, class... Args>
    void emplace(Args&&... args) requires Constructible<T, Args...> {

        using U = std::decay_t<T>;

        reset();

        Executor<U>::construct(this, std::forward<Args>(args)...);
        executor = &Executor<U>::execute;

    }


    void swap(Any& other) {

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

    template<class T>
    T get() {

        using U = std::decay_t<T>;
        static_assert(std::is_same<std::remove_cv_t<T>, U> && &Executor<U>::execute == executor, "Illegal direct any access");

        if (!hasValue() || typeid(U) != [this]() -> SizeT { if(!hasValue()) { return typeid(void).hash_code(); } Argument arg; executor(this, Operation::TypeInfo, &arg); return arg.typeHash;}()) {
            throw BadAnyAccess;
        } else {
            return Executor<U>::get(this);
        }

    }

    template<class T>
    T fastGet() {

        using U = std::decay_t<T>;
        static_assert(std::is_same<std::remove_cv_t<T>, U> && &Executor<U>::execute == executor, "Illegal direct any access");

        return Executor<U>::get(this);

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
        SizeT typeHash;
    };

    typedef void(*StateExecutor)(const Any*, Operation, Argument*);

    union Storage {

        constexpr Storage() : ptr(nullptr) {}

        constexpr Storage(const Storage& other) = delete;
        constexpr Storage& operator=(const Storage& other) = delete;

        void* ptr;
        alignas(void*) u8 buffer[sizeof(void*)];

    } storage;

    StateExecutor executor;
    constexpr static SizeT bufferSize = sizeof(Storage::buffer);
    constexpr static AlignT bufferAlignment = alignof(Storage);

    
    template<CopyConstructible T>
    struct Executor {

        //Condition: a) must fit into the buffer; b) alignment must be no stricter than those of buffer
        constexpr static bool StaticAllocatable = sizeof(T) <= bufferSize && alignof(T) <= bufferAlignment;

        template<class... Args>
        static void construct(Any* any, Args&&... args) requires Constructible<T, Args...> {

            if constexpr (StaticAllocatable) {
                ::new(any->storage.buffer) T(std::forward<Args>(args)...);
            } else {
                any->storage.ptr = new T(std::forward<Args>(args)...);
            }

        }

        static T& get(const Any* any) {
            
            if constexpr (StaticAllocatable) {
                return *reinterpret_cast<T*>(any->storage.buffer);
            } else {
                return static_cast<T*>(any->storage.ptr);
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
                    arg->typeHash = typeid(T).hash_code();
                    break;

            }

        }

    };

};