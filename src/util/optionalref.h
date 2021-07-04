#pragma once

#include <stdexcept>


/*
    Exception for bad OptionalRef access
*/
class BadOptionalRefException : public std::exception {

public:

    virtual const char* what() const noexcept {
        return "Accessed an OptionalRef that doesn't hold a reference";
    }

};


/*
    Optional reference that is immutable (i.e. cannot rebind)
*/
template<class T>
class OptionalRef {

public:

    static_assert(BaseType<T>, "T must be a plain data type");

    constexpr OptionalRef() noexcept : storage(nullptr), valid(false) {}
    constexpr OptionalRef(T& ref) noexcept : storage(&ref), valid(true) {}
    constexpr OptionalRef(const OptionalRef& other) noexcept : storage(other.storage), valid(other.valid) {}
    
    constexpr OptionalRef& operator=(const OptionalRef& other) noexcept {
        storage = other.storage;
        valid = other.valid;
        return *this;
    }

    constexpr bool has() const noexcept {
        return valid;
    }

    constexpr T& get() const {

        if(!has()) {
            throw BadOptionalRefException{};
        }

        return *storage;

    }

    template<class U>
    constexpr T& getOr(U& defaultValue) const noexcept {

        static_assert(std::is_convertible_v<U&&, T&>, "U& must be convertible to T&");

        if(!has()) {
            return static_cast<T&>(defaultValue);
        }

        return *storage;

    }

    template<class U>
    constexpr const T& getOr(const U& defaultValue) const noexcept {

        static_assert(std::is_convertible_v<U&&, const T&>, "const U& must be convertible to const T&");

        if(!has()) {
            return static_cast<const T&>(defaultValue);
        }

        return *storage;

    }

    constexpr void swap(OptionalRef& other) noexcept {

        if (has() || other.has()) {

            if(!(has() && other.has())) {
                valid = !valid;
                other.valid = !valid;
            }

            std::swap(storage, other.storage);

        }

    }

    constexpr void reset() noexcept {

        storage = nullptr;
        valid = false;

    }

    constexpr explicit operator bool() const noexcept {
        return has();
    }

    constexpr T& operator->() const noexcept {
        return *storage;
    }

    constexpr T& operator*() const noexcept {
        return *storage;
    }

    constexpr bool operator==(const OptionalRef<T>& other) const noexcept = default;

private:

    T* storage;
    bool valid;

};


namespace std {

    template <class T>
    constexpr void swap(OptionalRef<T>& left, OptionalRef<T>& right) noexcept {
        left.swap(right);
    }

}
