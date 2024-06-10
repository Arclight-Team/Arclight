/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Profiler.hpp
 */

#pragma once

#include "Timer.hpp"
#include "Meta/Concepts.hpp"

#include <string>


#define arc_profile(...) __Profilers::ProfileStatement(__VA_ARGS__) << [&]() /* { ... }; */


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


class ScopedProfiler : private Profiler
{
public:

	explicit ScopedProfiler(const std::string& name = "", Time::Unit unit = Time::Unit::Microseconds, u32 resolution = 3);
	~ScopedProfiler();
		
	using Profiler::setResolution;

private:

	std::string name;

};


namespace __Profilers {

	struct ProfileStatement : ScopedProfiler {

		using ScopedProfiler::ScopedProfiler;

		template<CC::Returns<void> F>
		constexpr void operator<<(F&& func) {
			func();
		}

	};

}
