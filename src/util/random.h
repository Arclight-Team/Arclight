#pragma once

#include "types.h"
#include <random>


class Random {

public:
	
	Random();
	Random(u64 seed);

	i64 getInt();
	i64 getInt(i64 min, i64 max);
	u64 getUint();
	u64 getUint(u64 min, u64 max);

	static Random& getRandom();

private:
	std::mt19937_64 rng;

};

