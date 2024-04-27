/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 MonitorManager.cpp
 */

#include "Monitor/MonitorManager.hpp"
#include "Monitor/Monitor.hpp"
#include "Monitor/MonitorHandle.hpp"
#include "Monitor/MonitorManagerHandle.hpp"
#include "Common/Win32.hpp"
#include "OS/Common.hpp"

MonitorManager::MonitorManager() = default;

MonitorManager::~MonitorManager() = default;

void MonitorManager::create() {

    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hMonitor, HDC hDC, LPRECT pRect, LPARAM pData) -> BOOL {

        auto* this_ = reinterpret_cast<MonitorManager*>(pData);

        auto mi = std::make_unique<MONITORINFOEXW>();
        mi->cbSize = sizeof(MONITORINFOEXW);

        if (!GetMonitorInfoW(hMonitor, mi.get())) {
            return true;
        }

        auto monitor = MonitorManagerHandle::createMonitor(hMonitor, hDC, std::move(mi));
        this_->monitors.emplace_back(std::move(monitor));

        return true;

    }, reinterpret_cast<LPARAM>(this));

}

void MonitorManager::pollEvents() {

    struct Vars {

        MonitorManager* this_;
        std::vector<const WCHAR*> availableMonitorNames;

    } vars;

    vars.this_ = this;

    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hMonitor, HDC hDC, LPRECT pRect, LPARAM pData) -> BOOL {

        auto* vars = reinterpret_cast<Vars*>(pData);
        auto* this_ = vars->this_;
        auto* availableMonitorNames = &vars->availableMonitorNames;

        auto mi = std::make_unique<MONITORINFOEXW>();
        mi->cbSize = sizeof(MONITORINFOEXW);

        if (!GetMonitorInfoW(hMonitor, mi.get())) {
            return true;
        }

        availableMonitorNames->emplace_back(mi->szDevice);

        if (auto found = MonitorManagerHandle::getMonitorForHMONITORName(*this_, mi->szDevice)) {
            // Existing monitor

            Monitor& prevMonitor = *found;
            MonitorHandle* prevHandle = prevMonitor.handle.get();
            auto& prevMi = prevHandle->info;

            prevHandle->hMonitor = hMonitor;
            prevHandle->hDC = hDC;

            bool changed =
                (memcmp(&prevMi->rcMonitor, &mi->rcMonitor, sizeof(RECT)) != 0) ||
                (memcmp(&prevMi->rcWork, &mi->rcWork, sizeof(RECT)) != 0) ||
                (prevMi->dwFlags != mi->dwFlags);

            if (changed) {
                prevHandle->info = std::move(mi);

                if (this_->monitorCallback) {
                    this_->monitorCallback(prevMonitor, MonitorManager::State::Changed);
                }
            }

        } else {
            // New monitor

            auto monitor = MonitorManagerHandle::createMonitor(hMonitor, hDC, std::move(mi));
            Monitor* monitorPtr = monitor.get();
            this_->monitors.emplace_back(std::move(monitor));

            if (this_->monitorCallback) {
                this_->monitorCallback(*monitorPtr, MonitorManager::State::Added);
            }

        }

        return true;

    }, reinterpret_cast<LPARAM>(&vars));

    std::erase_if(monitors, [&vars](auto& monitor){

        for (const WCHAR* monitorName : vars.availableMonitorNames) {
            if (wcscmp(monitorName, monitor->handle->info->szDevice) == 0) {
                return false;
            }
        }

        if (vars.this_->monitorCallback) {
            vars.this_->monitorCallback(*monitor, MonitorManager::State::Removed);
        }

        return true;

    });

}

OptionalRef<Monitor> MonitorManagerHandle::getMonitorForHMONITOR(MonitorManager& manager, HMONITOR hMonitor) {

    for (std::unique_ptr<Monitor>& monitor : manager.monitors) {
        if (monitor->handle->hMonitor == hMonitor) {
            return { *monitor };
        }
    }

    return {};

}

OptionalRef<Monitor> MonitorManagerHandle::getMonitorForHMONITORName(MonitorManager& manager, const WCHAR* name) {

    for (std::unique_ptr<Monitor>& monitor : manager.monitors) {
        if (wcscmp(monitor->handle->info->szDevice, name) == 0) {
            return { *monitor };
        }
    }

    return {};

}

std::unique_ptr<Monitor> MonitorManagerHandle::createMonitor(HMONITOR hMonitor, HDC hDC, std::unique_ptr<MONITORINFOEXW> mi) {

    auto monitor = std::unique_ptr<Monitor>(new Monitor());
    auto handle = std::make_unique<MonitorHandle>();

    handle->hMonitor = hMonitor;
    handle->hDC = hDC;
    handle->info = std::move(mi);

    monitor->handle = std::move(handle);
    return monitor;

}
