#pragma once

#include "util/timer.h"
#include <string>


class Profiler {

public:

	constexpr Profiler() : timer(), unit(Timer::Unit::Microseconds), resolution(3) {}
	constexpr Profiler(Timer::Unit unit, u32 resolution) : timer(), unit(unit), resolution(resolution) {}

	void start();
	void stop(const std::string& name = "");
	void setResolution(Timer::Unit unit, u32 resolution);

private:
	const char* getResolutionUnit() const;

	Timer timer;
	Timer::Unit unit;
	u32 resolution;

};