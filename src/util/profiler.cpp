#include "util/profiler.h"
#include "util/assert.h"

#include <chrono>
#include <cmath>


constexpr const char* resToUnit[] = {
	"s", "ms", "us", "ns"
};


void Profiler::setResolution(Resolution res, u32 decimalDigits) {
	arc_assert(decimalDigits < 7, "Too many decimal digits specified");
	resolution = res;
	digits = decimalDigits;
}



void Profiler::start() {
	startTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}



void Profiler::stop(const std::string& name) {

	arc_assert(startTime, "Profiler timer must be started first");

	u64 stopTime = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	u64 deltaNS = stopTime - startTime;

	u32 res = static_cast<u32>(resolution);
	double delta = deltaNS / static_cast<double>(std::pow(1000, 3 - res));

	if (name.empty()) {
		Log::debug("Profiler", "Profiler measured %0." + std::to_string(digits) + "f" + getResolutionUnit(), delta);
	} else {
		Log::debug("Profiler", "Profiler [%s] measured %0." + std::to_string(digits) + "f" + getResolutionUnit(), name.c_str(), delta);
	}

}


const char* Profiler::getResolutionUnit() const {
	return resToUnit[static_cast<u32>(resolution)];
}