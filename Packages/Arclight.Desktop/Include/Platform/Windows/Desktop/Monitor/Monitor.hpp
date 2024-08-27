/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Monitor.hpp
 */

#pragma once

#include "Common/Types.hpp"
#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"

#include <optional>
#include <string>
#include <memory>

class Window;
class MonitorHandle;

class Monitor {
public:
	Monitor();
	~Monitor();

	Monitor(Monitor&&) = default;
	Monitor& operator=(Monitor&&) = default;

	bool fromPrimary();
	bool fromWindow(Window& window);
	void destroy();

	// The virtual position of the monitor in pixels
	Vec2i getPosition() const;

	// The size of the monitor in pixels
	Vec2ui getSize() const;

	// The real size of the monitor in millimeters
	std::optional<Vec2ui> getPhysicalSize() const;

	// The area excluding the taskbar and other docked bars
	Rectangle<int> getWorkArea() const;

	// The refresh rate of this monitor
	u32 getRefreshRate() const;

	// The name of this monitor
	std::optional<std::string> getName() const;

	MonitorHandle& getInternalHandle();

private:
	std::unique_ptr<MonitorHandle> handle;
	friend MonitorHandle;
};
