/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 exception.hpp
 */

#pragma once

#include "typetraits.hpp"
#include "stacktrace.hpp"

#include <stdexcept>
#include <functional>



class ArclightException : public std::exception {

public:

	ArclightException() noexcept : ArclightException("Internal error") {}
	explicit ArclightException(const char* msg) noexcept : st(1), message(msg) {}
	explicit ArclightException(const std::string& msg) : st(1), message(msg) {}

	virtual const char* what() const noexcept override {
		return message.c_str();
	}

	virtual const char* name() const noexcept {
		return "Arclight Exception";
	}

	const Stacktrace& stacktrace() const noexcept {
		return st;
	}

private:

	Stacktrace st;
	std::string message;

};



class UnsupportedOperationException : public ArclightException {

public:

	using ArclightException::ArclightException;
	UnsupportedOperationException() noexcept : ArclightException("Operation not supported") {}

	virtual const char* name() const noexcept override { return "Unsupported Operation Exception"; }

};



namespace Exception {

	namespace Detail {

		template<class Dest, class Src>
		const Dest* tryDynamicCast(Src&& ex) noexcept {

			using T = TT::RemoveRef<Src>;

			const Dest* e = nullptr;

			if constexpr (CC::PointerType<T>) {
				e = dynamic_cast<const Dest*>(ex);
			} else {
				e = dynamic_cast<const Dest*>(&ex);
			}

			return e;

		}

	}

	template<class Exception>
	std::string getMessage(Exception&& ex) noexcept {

		try {

			const ArclightException* e = Detail::tryDynamicCast<ArclightException>(std::forward<Exception>(ex));

			if (e) {

				return std::string(e->name()) + ": " + e->what();

			} else {

				if constexpr (CC::BaseOf<TT::RemoveCV<TT::RemovePointer<TT::RemoveCVRef<Exception>>>, std::exception>) {
					return std::string("Std Exception: ") + std::bind(&std::exception::what, ex)();
				}

				return std::string("Object of type ") + typeid(ex).name();

			}

		} catch (...) {

			return "Fatal error";

		}

	}

	template<class Exception>
	std::string getStackTrace(Exception&& ex) noexcept {

		try {

			const ArclightException* e = Detail::tryDynamicCast<ArclightException>(std::forward<Exception>(ex));

			if (e) {

				//Print Arclight Exception
				return "Stacktrace:\n" + e->stacktrace().toString();

			} else {

				//Get the stack trace from here
				Stacktrace st(2);
				return "Stacktrace (incomplete):\n" + st.toString();

			}

		} catch (...) {

			return "???";

		}

	}

}