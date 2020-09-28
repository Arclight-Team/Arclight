#include "util/timer.h"
#include "util/assert.h"

#include <chrono>
#include <cmath>



void Timer::start() {
	startTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}



double Timer::getElapsedTime(Unit unit) const {

	arc_assert(startTime, "Profiler timer must be started first");

	u64 stopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	u64 deltaNS = stopTime - startTime;

	u32 res = static_cast<u32>(unit);
	double delta = deltaNS / static_cast<double>(std::pow(1000, 3 - res));

	return delta;

}