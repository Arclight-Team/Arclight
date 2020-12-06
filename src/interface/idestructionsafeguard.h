#pragma once

#include "util/assert.h"


class IDestructionSafeguard {

public:

#ifndef NDEBUG
	inline virtual ~IDestructionSafeguard() {
		arc_assert(locked, "Safeguard lock tripped");
	}
#endif

	inline void aquireLock() {
#ifndef NDEBUG
		arc_assert(!locked, "Safeguard lock re-aquired");
		locked = true;
#endif
	}

	inline void releaseLock() {
#ifndef NDEBUG
		arc_assert(locked, "Safeguard lock re-released");
		locked = false;
#endif
	}

private:

	bool locked = false;

};