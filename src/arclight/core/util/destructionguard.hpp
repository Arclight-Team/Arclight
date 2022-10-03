/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 destructionguard.hpp
 */

#pragma once

#include "util/log.hpp"

#include <functional>



class DestructionGuard {

public:

	DestructionGuard(const std::function<void()>& func) : dtor(func) {}

	~DestructionGuard() {

		try {
			dtor();
		} catch (...) {
			LogE("Guard") << "Fatal exception thrown in destructor";
		}

	}

private:

	std::function<void()> dtor;

};