/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 optionalref.hpp
 */

#pragma once

#include "common/typetraits.hpp"
#include "common/concepts.hpp"
#include "common/exception.hpp"

#include <stdexcept>


/*
	Exception for bad OptionalRef access
*/
class BadOptionalRefException : public ArclightException {

public:
	virtual const char* name() const noexcept override { return "Bad OptionalRef Exception"; }
	virtual const char* what() const noexcept override { return "Illegal optional access"; }

};


/*
	Optional reference that will rebind upon assignment
*/
template<class T>
class OptionalRef {

public:

	static_assert(CC::BaseType<TT::RemoveCV<T>>, "T must be a plain data type");

	constexpr OptionalRef() noexcept : storage(nullptr), valid(false) {}
	constexpr OptionalRef(T& ref) noexcept : storage(&ref), valid(true) {}

	template<class U> requires(CC::Convertible<U&, T&>)
	constexpr OptionalRef(const OptionalRef<U>& other) noexcept : storage(other.storage), valid(other.valid) {}
		
	template<class U> requires(CC::Convertible<U&, T&>)
	constexpr OptionalRef& operator=(const OptionalRef<U>& other) noexcept {
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

	constexpr T& getDirect() const noexcept {
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

	constexpr T* operator->() const noexcept {
		return storage;
	}

	constexpr T& operator*() const noexcept {
		return *storage;
	}

	constexpr bool operator==(const OptionalRef<T>& other) const noexcept = default;

private:

	template<class U>
	friend class OptionalRef;

	T* storage;
	bool valid;

};



template<class O, class I>
OptionalRef<O> optionalRefCast(const OptionalRef<I>& in) {

	if (!in.has())
		return {};

	return OptionalRef<O>(static_cast<O&>(in.get()));

}


namespace std {

	template<class T>
	constexpr void swap(OptionalRef<T> &left, OptionalRef<T> &right) noexcept {
		left.swap(right);
	}

}