/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 MonitorManager.hpp
 */

#pragma once

#include <memory>
#include <vector>
#include <functional>

class Monitor;
class MonitorManagerHandle;

class MonitorManager {
public:
    enum class State {
        Added,
        Removed,
        Changed
    };

    using MonitorCallback = std::function<void(Monitor&, State)>;

    MonitorManager();
    ~MonitorManager();

    void create();
    void pollEvents();

    inline void setMonitorCallback(MonitorCallback callback) {
        monitorCallback = std::move(callback);
    }

private:
    std::vector<std::unique_ptr<Monitor>> monitors;
    MonitorCallback monitorCallback;

    friend MonitorManagerHandle;
};
