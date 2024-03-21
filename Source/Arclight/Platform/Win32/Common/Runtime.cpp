/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Runtime.cpp
 */

#include "Util/Log.hpp"
#include "OS/OS.hpp"



namespace ArcRuntime {

	bool platformInit() {
#ifdef ARC_CFG_ARCRT_ENABLE
		return OS::init();
#else
		return true;
#endif
	}

}
