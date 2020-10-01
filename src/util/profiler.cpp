#include "util/profiler.h"
#include "util/assert.h"


constexpr const char* resToUnit[] = {
	"s", "ms", "us", "ns"
};


void Profiler::setResolution(Timer::Unit unit, u32 resolution) {

	arc_assert(resolution < 7, "Too many decimal digits specified");
	this->unit = unit;
	this->resolution = resolution;

}



void Profiler::start() {
	timer.start();
}



void Profiler::stop(const std::string& name) {

	double delta = timer.getElapsedTime(unit);

	if (name.empty()) {
		Log::debug("Profiler", "Profiler measured %0." + std::to_string(resolution) + "f" + getResolutionUnit(), delta);
	} else {
		Log::debug("Profiler", "Profiler [%s] measured %0." + std::to_string(resolution) + "f" + getResolutionUnit(), name.c_str(), delta);
	}

}


const char* Profiler::getResolutionUnit() const {
	return resToUnit[static_cast<u32>(unit)];
}