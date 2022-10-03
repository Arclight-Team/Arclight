/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 profiler.cpp
 */

#include "profiler.hpp"
#include "util/assert.hpp"
#include "arcconfig.hpp"


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
		LogI("Profiler").print("Measured %0." + std::to_string(resolution) + "f" + Time::getUnitSuffix(unit), delta);
	} else {
		LogI("Profiler").print("%-10s Measured %0." + std::to_string(resolution) + "f" + Time::getUnitSuffix(unit), name.c_str(), delta);
	}

}