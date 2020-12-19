#include "time.h"
#include "math.h"
#include "assert.h"

#include <chrono>
#include <ctime>


namespace Time {

	typedef std::chrono::system_clock SystemClock;

	constexpr const char* unitSuffixes[] = {
		"s", "ms", "us", "ns"
	};

	constexpr const char* timestampSeparator = "-";


	std::string getTimeValueString(u32 value) {

		if (value < 10) {
			return "0" + std::to_string(value);
		} else {
			return std::to_string(value);
		}

	}



	template<typename T, typename P>
	u64 timeCount(Time::Unit unit, const std::chrono::duration<T, P>& duration) {
		
		switch (unit) {

			case Time::Unit::Seconds:
				return std::chrono::duration_cast<std::chrono::seconds>(duration).count();

			case Time::Unit::Milliseconds:
				return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

			case Time::Unit::Microseconds:
				return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

			case Time::Unit::Nanoseconds:
				return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

			default:
				arc_force_assert("Invalid time unit %d", static_cast<u32>(unit));
				return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

		}

	}



	u64 getTimeSinceEpoch(Time::Unit unit) {
		return timeCount(unit, SystemClock::now().time_since_epoch());
	}



	TimeData getCurrentTime() {

		auto time = SystemClock::to_time_t(SystemClock::now());
		std::tm* timeData = std::localtime(&time);

		return TimeData(timeData->tm_sec, timeData->tm_min, timeData->tm_hour, timeData->tm_mday, timeData->tm_mon + 1, timeData->tm_year + 1900);

	}



	const char* getUnitSuffix(Time::Unit unit) {
		return unitSuffixes[static_cast<u32>(unit)];
	}



	double getUnitFactor(Time::Unit unit) {
		return Math::pow(1000, static_cast<u32>(unit));
	}



	double convert(u64 time, Time::Unit srcUnit, Time::Unit destUnit) {

		double srcFactor = getUnitFactor(srcUnit);
		double destFactor = getUnitFactor(destUnit);
		double totalFactor = destFactor / srcFactor;

		return time * totalFactor;

	}



	std::string getTimestamp() {

		TimeData data = getCurrentTime();

		return	getTimeValueString(data.year) + timestampSeparator + getTimeValueString(data.month) + timestampSeparator + getTimeValueString(data.day) + timestampSeparator +
				getTimeValueString(data.hour) + timestampSeparator + getTimeValueString(data.minute) + timestampSeparator + getTimeValueString(data.second);

	}

}