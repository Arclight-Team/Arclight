#pragma once

#include "timer.hpp"


class FPSTracker : public Timer {

public:

	constexpr FPSTracker() : frames(0), fps(0) {}

	double getFPS();

private:
	 
	u32 frames;
	double fps;

};