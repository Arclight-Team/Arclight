/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 timer.hpp
 */

#pragma once

#include "time.hpp"
#include "common/types.hpp"


class Timer {

public:

	constexpr Timer() : startTime(0) {}

	void start();
	double getElapsedTime(Time::Unit unit = Time::Unit::Microseconds) const;

private:
	u64 startTime;

};