#pragma once

#include "timer.h"


class Ticker {

public:

    Ticker();

    void start(u32 tps);
    u32 getTicks();

private:

    Timer timer;
    u32 nsPerTick;
	u64 accumulator;

};