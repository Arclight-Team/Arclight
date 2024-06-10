/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Win32.hpp
 */

#pragma once

#ifndef NOMINMAX // Clashing Math/Math.hpp
	#define NOMINMAX
#endif

#include <Windows.h>

#ifdef MessageBox // Clashing System/MessageBox.hpp
	#undef MessageBox
#endif

#ifdef min
	#undef min
#endif

#ifdef max
	#undef max
#endif
