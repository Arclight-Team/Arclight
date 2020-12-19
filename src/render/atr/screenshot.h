#pragma once

#include "types.h"


namespace Screenshot {

	constexpr u32 maxScreenshotsPerTimestamp = 5;

	bool save(u32 w, u32 h, u8* data);

}