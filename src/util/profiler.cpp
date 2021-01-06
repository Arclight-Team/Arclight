#include "util/profiler.h"
#include "util/assert.h"



void Profiler::setResolution(Time::Unit unit, u32 resolution) {

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
		Log::info("Profiler", "Profiler measured %0." + std::to_string(resolution) + "f" + Time::getUnitSuffix(unit), delta);
	} else {
		Log::info("Profiler", "Profiler [%s] measured %0." + std::to_string(resolution) + "f" + Time::getUnitSuffix(unit), name.c_str(), delta);
	}

}