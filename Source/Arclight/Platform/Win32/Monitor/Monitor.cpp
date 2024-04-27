/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Monitor.cpp
 */

#define Rectangle Rectangle_
#include "Common/Win32.hpp" // Force disable GDI's rectangle function
// Must be included before WindowHandle
#undef Rectangle

#include "Monitor/Monitor.hpp"
#include "Monitor/MonitorHandle.hpp"
#include "OS/Common.hpp"
#include "OS/Hardware.hpp"

Monitor::~Monitor() = default;

Vec2i Monitor::getPosition() const {

    RECT& rcMonitor = handle->info->rcMonitor;
    return {
        rcMonitor.left,
        rcMonitor.top
    };

}

Vec2ui Monitor::getSize() const {

    RECT& rcMonitor = handle->info->rcMonitor;
    return {
        rcMonitor.right - rcMonitor.left,
        rcMonitor.bottom - rcMonitor.top
    };

}

Vec2ui Monitor::getPhysicalSize() const {

    HDC hDC = handle->hDC;
    RECT& rcMonitor = handle->info->rcMonitor;

    u32 widthMM, heightMM;
    if (OS::Hardware::getOSVersion() >= OS::Hardware::Version(8, 1, 0)) {
        widthMM = static_cast<u32>(GetDeviceCaps(hDC, HORZSIZE));
        heightMM = static_cast<u32>(GetDeviceCaps(hDC, VERTSIZE));
    } else {
        u32 width = rcMonitor.right - rcMonitor.left;
        u32 height = rcMonitor.bottom - rcMonitor.top;
        widthMM = static_cast<u32>(static_cast<float>(width) * 25.4f / static_cast<float>(GetDeviceCaps(hDC, LOGPIXELSX)));
        heightMM = static_cast<u32>(static_cast<float>(height) * 25.4f / static_cast<float>(GetDeviceCaps(hDC, LOGPIXELSY)));
    }

    return { widthMM, heightMM };

}

Rectangle<int> Monitor::getWorkArea() const {

    RECT& rcWork = handle->info->rcWork;
    return {
        rcWork.left,
        rcWork.top,
        rcWork.right - rcWork.left,
        rcWork.bottom - rcWork.top
    };

}

u32 Monitor::getRefreshRate() const {

    return static_cast<u32>(GetDeviceCaps(handle->hDC, VREFRESH));

}

std::optional<std::string> Monitor::getName() const {

    return OS::String::toUTF8(handle->info->szDevice);

}
