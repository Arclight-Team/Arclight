#pragma once

#include "types.h"

#include <string>


class Profiler {

public:

	enum class Resolution {
		Seconds = 0,
		Milliseconds,
		Microseconds,
		Nanoseconds
	};

	constexpr Profiler() : startTime(0), resolution(Resolution::Microseconds), digits(3) {}
	constexpr Profiler(Resolution res, u32 decimalDigits) : startTime(0), resolution(res), digits(decimalDigits) {}

	void start();
	void stop(const std::string& name = "");
	void setResolution(Resolution res, u32 decimalDigits);

private:
	const char* getResolutionUnit() const;

	u64 startTime;
	Resolution resolution;
	u32 digits;

};