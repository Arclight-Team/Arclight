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
#include "Window/Window.hpp"
#include "Window/WindowHandle.hpp"
#include "OS/Common.hpp"
#include "OS/Hardware.hpp"

Monitor::Monitor() = default;
Monitor::~Monitor() = default;

bool Monitor::fromPrimary() {

	POINT ptZero = { 0, 0 };
	HMONITOR hMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

	return MonitorHandle::fromNativeHandle(*this, hMonitor);

}

bool Monitor::fromWindow(Window& window) {

	arc_assert(window.isOpen(), "Tried to get monitor for non-existing window");

	auto windowHandle = window.getInternalHandle().lock();

	if (!windowHandle) {
		LogE("Monitor") << "Failed to obtain window handle";
		return false;
	}

	HWND hWnd = windowHandle->getHWND();
	HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

	return MonitorHandle::fromNativeHandle(*this, hMonitor);

}

void Monitor::destroy() {

	handle.reset();

}

Vec2i Monitor::getPosition() const {

	RECT& rcMonitor = handle->mi.rcMonitor;
	return {
		rcMonitor.left,
		rcMonitor.top
	};

}

Vec2ui Monitor::getSize() const {

	RECT& rcMonitor = handle->mi.rcMonitor;
	return {
		rcMonitor.right - rcMonitor.left,
		rcMonitor.bottom - rcMonitor.top
	};

}

std::optional<Vec2ui> Monitor::getPhysicalSize() const {

	HDC hDC = handle->createDC();
	if (!hDC) {
		return {};
	}

	RECT& rcMonitor = handle->mi.rcMonitor;

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

	DeleteDC(hDC);

	return {{ widthMM, heightMM }};

}

Rectangle<int> Monitor::getWorkArea() const {

	RECT& rcWork = handle->mi.rcWork;
	return {
		rcWork.left,
		rcWork.top,
		rcWork.right - rcWork.left,
		rcWork.bottom - rcWork.top
	};

}

u32 Monitor::getRefreshRate() const {

	return handle->refreshRate;

}

std::optional<std::string> Monitor::getName() const {

	return OS::String::toUTF8(handle->mi.szDevice);

}

MonitorHandle& Monitor::getInternalHandle() {

	arc_assert(handle != nullptr, "Tried to get internal handle for non-existing monitor");

	return *handle;

}

bool MonitorHandle::fromNativeHandle(Monitor& m, HMONITOR hMonitor) {

	m.handle = std::make_unique<MonitorHandle>();
	MonitorHandle& mh = *m.handle;

	mh.mi.cbSize = sizeof(MONITORINFOEX);
	if (!GetMonitorInfoW(hMonitor, &mh.mi)) {
		return false;
	}

	HDC hDC = mh.createDC();
	if (!hDC) {
		return false;
	}

	mh.refreshRate = static_cast<u32>(GetDeviceCaps(hDC, VREFRESH));

	DeleteDC(hDC);

	return true;

}

HDC MonitorHandle::createDC() {

	return CreateDCW(L"DISPLAY", mi.szDevice, nullptr, nullptr);

}
