#pragma once

#include "types.h"
#include "time.h"


class Timer {

public:

	constexpr Timer() : startTime(0) {}

	void start();
	double getElapsedTime(Time::Unit unit = Time::Unit::Microseconds) const;

private:
	u64 startTime;

};