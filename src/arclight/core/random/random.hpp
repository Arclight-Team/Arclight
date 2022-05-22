/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 random.hpp
 */

#pragma once

#include "common/concepts.hpp"



class IRandomNumberGenerator {};

template<class T>
concept RandomNumberGenerator = CC::BaseOf<IRandomNumberGenerator, T> && requires {
	T::SeedBits -> SizeT;
};