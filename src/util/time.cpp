#include "time.h"
#include "math/math.h"
#include "util/assert.h"

#include <chrono>


namespace Time {

	typedef std::chrono::high_resolution_clock Clock;

	constexpr const char* unitSuffixes[] = {
		"s", "ms", "us", "ns"
	};

	constexpr const char* timestampSeparator = "-";


	std::string getTimeValueString(u32 value) {

		if (value < 10) {
			return '0' + std::to_string(value);
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
		return timeCount(unit, Clock::now().time_since_epoch());
	}



	TimeData getCurrentTime() {

		auto now = Clock::now();
		std::chrono::hh_mm_ss hms(now.time_since_epoch());
		std::chrono::year_month_day ymd(std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now()));

		return TimeData(hms.seconds().count(), hms.minutes().count(), hms.hours().count(), u32(ymd.day()), u32(ymd.month()), i32(ymd.year()));

	}



	const char* getUnitSuffix(Time::Unit unit) {
		return unitSuffixes[static_cast<u32>(unit)];
	}



#if 0
	double getUnitFactor(Time::Unit unit) {
		return Math::pow(1000, static_cast<u32>(unit));
	}



	double convert(u64 time, Time::Unit srcUnit, Time::Unit destUnit) {

		double srcFactor = getUnitFactor(srcUnit);
		double destFactor = getUnitFactor(destUnit);
		double totalFactor = destFactor / srcFactor;

		return time * totalFactor;

	}
#endif



	std::string getTimestamp() {

		TimeData data = getCurrentTime();

		return	getTimeValueString(data.year) + timestampSeparator + getTimeValueString(data.month) + timestampSeparator + getTimeValueString(data.day) + timestampSeparator +
				getTimeValueString(data.hour) + timestampSeparator + getTimeValueString(data.minute) + timestampSeparator + getTimeValueString(data.second);

	}

}