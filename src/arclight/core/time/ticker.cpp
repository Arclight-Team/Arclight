/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 ticker.cpp
 */

#include "ticker.hpp"


Ticker::Ticker() : usPerTick(1), accumulator(0) {}


void Ticker::start(u32 tps) {

	timer.start();
	accumulator = 0;
	usPerTick = 1000000ULL / tps;

}



u64 Ticker::getTicks() {

	u64 delta = static_cast<u64>(timer.getElapsedTime(Time::Unit::Microseconds));
	timer.start();

	accumulator += delta;
	u64 ticks = 0;

	if (accumulator > usPerTick) {

		ticks = accumulator / usPerTick;
		accumulator %= usPerTick;

	}

	return ticks;

}