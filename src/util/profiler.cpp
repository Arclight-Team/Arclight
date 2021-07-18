#include "util/profiler.h"
#include "util/assert.h"
#include "arcconfig.h"


void Profiler::setResolution(Time::Unit unit, u32 resolution) {

#ifdef ARC_ENABLE_PROFILER
	arc_assert(resolution < 7, "Too many decimal digits specified");
	this->unit = unit;
	this->resolution = resolution;
#endif

}



void Profiler::start() {

#ifdef ARC_ENABLE_PROFILER
	timer.start();
#endif

}



void Profiler::stop(const std::string& name) {

#ifdef ARC_ENABLE_PROFILER
	double delta = timer.getElapsedTime(unit);

	if (name.empty()) {
		Log::info("Profiler", "Profiler measured %0." + std::to_string(resolution) + "f" + Time::getUnitSuffix(unit), delta);
	} else {
		Log::info("Profiler", "Profiler [%-10s] measured %0." + std::to_string(resolution) + "f" + Time::getUnitSuffix(unit), name.c_str(), delta);
	}
#endif

}