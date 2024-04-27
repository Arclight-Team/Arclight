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
private:
    HMONITOR hMonitor;
    HDC hDC;
    std::unique_ptr<MONITORINFOEXW> info;

    friend Monitor;
    friend MonitorManager;
    friend MonitorManagerHandle;
};
