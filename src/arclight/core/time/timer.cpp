#include "timer.hpp"
#include "util/assert.hpp"

#include <chrono>
#include <cmath>



void Timer::start() {
	startTime = Time::getTimeSinceEpoch(Time::Unit::Nanoseconds);
}



double Timer::getElapsedTime(Time::Unit unit) const {

	arc_assert(startTime, "Timer must be started first");

	u64 stopTime = Time::getTimeSinceEpoch(Time::Unit::Nanoseconds);
	u64 deltaNS = stopTime - startTime;
	double delta = Time::convert(deltaNS, Time::Unit::Nanoseconds, unit);

	return delta;

}