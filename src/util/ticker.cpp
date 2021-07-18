#include "ticker.h"


Ticker::Ticker() : nsPerTick(1), accumulator(0) {}


void Ticker::start(u32 tps) {

    timer.start();
	accumulator = 0;
    nsPerTick = 1000000ULL / tps;

}



u32 Ticker::getTicks() {

	u64 delta = static_cast<u64>(timer.getElapsedTime(Time::Unit::Microseconds));
    timer.start();

	accumulator += delta;
	u32 ticks = 0;

	if (accumulator > nsPerTick) {

		ticks = accumulator / nsPerTick;
		accumulator %= nsPerTick;

	}

    return ticks;

}