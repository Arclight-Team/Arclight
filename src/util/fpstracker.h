#pragma once

#include "timer.h"


class FPSTracker : public Timer {

public:

	constexpr FPSTracker() : frames(0), fps(0) {}

	double getFPS();

private:
	 
	u32 frames;
	double fps;

};