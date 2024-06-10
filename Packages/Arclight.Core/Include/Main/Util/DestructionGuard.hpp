/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 DestructionGuard.hpp
 */

#pragma once

#include "Util/Log.hpp"



template<CC::Invocable I>
class DestructionGuard {

public:

	explicit constexpr DestructionGuard(const I& function) noexcept(CC::NothrowCopyConstructible<I>) : dtor(function) {}

	constexpr ~DestructionGuard() noexcept(noexcept(std::declval<I>()())) {

		try {
			dtor();
		} catch (...) {
			LogE("Guard") << "Fatal exception thrown in destructor";
		}

	}

private:

	I dtor;

};

