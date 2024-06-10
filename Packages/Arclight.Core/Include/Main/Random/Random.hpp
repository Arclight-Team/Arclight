/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Random.hpp
 */

#pragma once

#include "Meta/Concepts.hpp"



class IRandomNumberGenerator {};

namespace CC {

	template<class T>
	concept RandomNumberGenerator = CC::BaseOf<T, IRandomNumberGenerator> && requires {
		T::SeedBits -> SizeT;
	};

}
