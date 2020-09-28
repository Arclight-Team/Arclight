#pragma once

#include "types.h"


class Timer {

public:

	enum class Unit {
		Seconds = 0,
		Milliseconds,
		Microseconds,
		Nanoseconds
	};

	constexpr Timer() : startTime(0) {}

	void start();
	double getElapsedTime(Unit unit = Unit::Microseconds) const;

private:
	u64 startTime;

};