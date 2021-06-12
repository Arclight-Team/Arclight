#pragma once

#include "types.h"


class BadAnyCast : public std::exception {

public:

    virtual const char* what() const noexcept override {
        return "Bad any cast";
    }

};



template<SizeT Size>
class Any {

    typedef void(*StateExecutor)(const Any* any, Operation operation, Argument* arg);

public:

    constexpr Any() : executor(nullptr) {}

    ~Any() {
        reset();
    }


    constexpr Any& operator=(const Any& other) {
        *this = Any(other);
        return *this;
    }


    constexpr Any& operator=(Any&& other) {

        if(!other.hasValue()) {
            reset();
        } else if(*this != other) {
            reset();
        }

    }


    constexpr bool hasValue() const noexcept {
        return executor != nullptr;
    }


    void reset() {

        if(hasValue()) {

            executor(this, Executor::Operation::Destruct, nullptr);
            executor = nullptr;

        }

    }


    void swap(Any& other) {

        if(!hasValue() && !other.hasValue()) {
            return;
        }

        if(hasValue() && other.hasValue()) {

            //Swap underlying contents
            Executor::Argument argument;
            Any temp;

            argument->any = &temp;
            other.executor(&other, Executor::Operation::Move, &argument);

            argument->any = &other;
            executor(this, Executor::Operation::Move, &argument);

            argument->any = this;
            temp.executor(&temp, Executor::Operation::Move, &argument);

        } else if (hasValue()) {

            //Transfer contents to other
            Executor::Argument argument;
            argument->any = &other;
            executor(this, Executor::Operation::Move, &argument);

        } else {

            //Receive contents from other
            Executor::Argument argument;
            argument->any = this;
            other.executor(&other, Executor::Operation::Move, &argument);

        }

    }

private:

    union Storage {

        constexpr Storage() : ptr(nullptr) {}

        constexpr Storage(const Storage& other) = delete;
        constexpr Storage& operator=(const Storage& other) = delete;

        void* ptr;
        alignas(ptr) u8 buffer[sizeof(ptr)];

    } storage;

    StateExecutor executor;

    
    template<class T>
    struct Executor {

        //Condition: a) must fit into the buffer; b) alignment must be no stricter than those of buffer
        constexpr static bool StaticAllocatable = sizeof(T) <= sizeof(Storage::buffer) && alignof(T) <= alignof(Storage::buffer);

        enum class Operation {
            Destruct,
            Move,
            Copy
        };

        union Argument {
            Any* any;
        };

        template<class... Args>
        static void create(const Any& any, Args&&... args) {

            if constexpr (StaticAllocatable) {
                ::new(any.storage.buffer)  T(std::forward<Args>(args)...);
            } else {
                any.storage.ptr = new T(std::forward<Args>(args)...);
            }

        }

        static T& get(const Any& any) {
            
            if constexpr (StaticAllocatable) {
                return *reinterpret_cast<T*>(any.storage.buffer);
            } else {
                return static_cast<T*>(any.storage.ptr);
            }

        }

        static void execute(const Any* any, Operation operation, Argument* arg) {

            switch(operation) {

                case Operation::Destruct:

                    if constexpr (StaticAllocatable) {
                        reinterpret_cast<T*>(any->storage.buffer)->~T();
                    } else {
                        delete static_cast<T*>(any->storage.ptr);
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
                        T* ptr = reinterpret_cast<T*>(from->storage.buffer);

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

            }

        }

    };

};