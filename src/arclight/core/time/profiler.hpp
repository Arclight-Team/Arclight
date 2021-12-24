#pragma once

#include "timer.hpp"

#include <string>


class Profiler {

public:

	constexpr Profiler() : timer(), unit(Time::Unit::Microseconds), resolution(3) {}
	constexpr Profiler(Time::Unit unit, u32 resolution) : timer(), unit(unit), resolution(resolution) {}

	void start();
	void stop(const std::string& name = "");
	void setResolution(Time::Unit unit, u32 resolution);

private:

	Timer timer;
	Time::Unit unit;
	u32 resolution;

};