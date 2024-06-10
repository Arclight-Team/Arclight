/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Ticker.hpp
 */

#pragma once

#include "Timer.hpp"


class Ticker {

public:

	Ticker();

	void start(u32 tps);
	u64 getTicks();

private:

	Timer timer;
	u32 usPerTick;
	u64 accumulator;

};