/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 fpstracker.hpp
 */

#pragma once

#include "timer.hpp"


class FPSTracker : public Timer {

public:

	constexpr FPSTracker() : frames(0), fps(0) {}

	double getFPS();

private:
	 
	u32 frames;
	double fps;

};