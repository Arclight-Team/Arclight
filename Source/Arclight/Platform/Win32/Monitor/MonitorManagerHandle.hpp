/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 MonitorManagerHandle.hpp
 */

#pragma once

#include <memory>

#include "Common/Win32.hpp"
#include "StdExt/OptionalRef.hpp"

class Monitor;
class MonitorManager;

class MonitorManagerHandle {
public:
    static OptionalRef<Monitor> getMonitorForHMONITOR(MonitorManager& manager, HMONITOR hMonitor);
    static OptionalRef<Monitor> getMonitorForHMONITORName(MonitorManager& manager, const WCHAR* name);
    static std::unique_ptr<Monitor> createMonitor(HMONITOR hMonitor, HDC hDC, std::unique_ptr<MONITORINFOEXW> mi);

private:
    friend MonitorManager;
};
