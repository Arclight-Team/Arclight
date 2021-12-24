#pragma once

#include "timer.hpp"


class Ticker {

public:

    Ticker();

    void start(u32 tps);
    u32 getTicks();

private:

    Timer timer;
    u32 usPerTick;
	u64 accumulator;

};