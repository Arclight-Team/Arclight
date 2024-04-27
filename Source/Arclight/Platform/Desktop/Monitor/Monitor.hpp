/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Monitor.hpp
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "Common/Types.hpp"
#include "Math/Vector.hpp"
#include "Math/Shapes/Rectangle.hpp"

class MonitorHandle;
class MonitorManager;
class MonitorManagerHandle;

class Monitor {
public:
    ~Monitor();

    // The virtual position of the monitor in pixels
    Vec2i getPosition() const;

    // The size of the monitor in pixels
    Vec2ui getSize() const;

    // The real size of the monitor in millimeters
    Vec2ui getPhysicalSize() const;

    // The area excluding the taskbar and other docked bars
    Rectangle<int> getWorkArea() const;

    // The refresh rate of this monitor
    u32 getRefreshRate() const;

    // The name of this monitor
    std::optional<std::string> getName() const;

    inline MonitorHandle& getInternalHandle() { return *handle; }

private:
    inline Monitor() = default;

    std::unique_ptr<MonitorHandle> handle;

    friend MonitorManager;
    friend MonitorManagerHandle;
};
