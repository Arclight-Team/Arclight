/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Timer.hpp
 */

#pragma once

#include "Time.hpp"
#include "Common/Types.hpp"


class Timer {

public:

	constexpr Timer() : startTime(0) {}

	void start();
	double getElapsedTime(Time::Unit unit = Time::Unit::Microseconds) const;

private:
	u64 startTime;

};