/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 random.hpp
 */

#pragma once

#include "meta/concepts.hpp"



class IRandomNumberGenerator {};

namespace CC {

	template<class T>
	concept RandomNumberGenerator = CC::BaseOf<T, IRandomNumberGenerator> && requires {
		T::SeedBits -> SizeT;
	};

}
