/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 MonitorHandle.hpp
 */

#pragma once

#include <memory>

#include "Common/Win32.hpp"

class Monitor;

class MonitorHandle {
public:
    static bool fromNativeHandle(Monitor& m, HMONITOR hMonitor);

private:
    //HMONITOR hMonitor;
    MONITORINFOEXW mi;
    u32 refreshRate;

    HDC createDC();

    friend Monitor;
};
