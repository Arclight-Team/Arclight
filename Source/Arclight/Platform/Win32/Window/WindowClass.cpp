/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowClass.cpp
 */

#include "Window/WindowClass.hpp"
#include "Window/WindowClassHandle.hpp"
#include "Window/WindowHandle.hpp"
#include "OS/Common.hpp"
#include "Common/Assert.hpp"
#include "Util/Log.hpp"

#include <Windows.h>

WindowClass::WindowClass() = default;
WindowClass::~WindowClass() = default;

bool WindowClass::create(std::string_view name) {

    handle = std::make_unique<WindowClassHandle>();
    handle->name = name;
    handle->wname = OS::String::toUTF16(name);

    HMODULE hinst = GetModuleHandleW(nullptr);

    WNDCLASSEXW wndClass;
    wndClass.cbSize        = sizeof(WNDCLASSEXW);
    wndClass.style         = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc   = WindowHandle::wndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hInstance     = hinst;
    wndClass.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
    wndClass.hIconSm       = LoadIconW(nullptr, IDI_APPLICATION);
    wndClass.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
    wndClass.hbrBackground = nullptr;
    wndClass.lpszMenuName  = nullptr;
    wndClass.lpszClassName = handle->wname.c_str();

    if (!RegisterClassExW(&wndClass)) {
        LogE("WindowClass") << "Failed to register window class";
        return false;
    }

    return true;

}

void WindowClass::destroy() {

    HMODULE hinst = GetModuleHandleW(nullptr);

    UnregisterClassW(handle->wname.c_str(), hinst);

}

const std::string& WindowClass::getName() const {

    arc_assert(handle != nullptr, "Tried to get name for non-existing window class");
    return handle->name;

}

WindowClassHandle& WindowClass::getInternalHandle() {

    arc_assert(handle != nullptr, "Tried to get internal handle for non-existing window class");
    return *handle;

}
