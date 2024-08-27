/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowClass.cpp
 */

#include "Desktop/Window/WindowClass.hpp"
#include "Desktop/Window/WindowHandle.hpp"
#include "System/Common.hpp"
#include "Common/Win32.hpp"



WindowClassPtr WindowClass::defaultClass = WindowClass::create("ArclightStandardWindowHandle");



WindowClass::WindowClass() : owning(false) {}

WindowClass::~WindowClass() {

	if (isValid() && owning) {

		UnregisterClassW(System::String::toUTF16(className).c_str(), GetModuleHandleW(nullptr));

		className.clear();
		owning = false;

	}

}



WindowClassPtr WindowClass::create(std::string_view name) {

	WindowClassPtr wc = std::make_shared<WindowClass>();
	wc->owning = true;
	wc->className = name;

	std::wstring wcn = System::String::toUTF16(name);

	WNDCLASSEXW wndClass;
	wndClass.cbSize        = sizeof(WNDCLASSEXW);
	wndClass.style         = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc   = WindowHandle::onWindowUpdate;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = GetModuleHandleW(nullptr);
	wndClass.hIcon         = LoadIconW(nullptr, IDI_APPLICATION);
	wndClass.hIconSm       = LoadIconW(nullptr, IDI_APPLICATION);
	wndClass.hCursor       = LoadCursorW(nullptr, IDC_ARROW);
	wndClass.hbrBackground = nullptr;
	wndClass.lpszMenuName  = nullptr;
	wndClass.lpszClassName = wcn.c_str();

	if (RegisterClassExW(&wndClass)) {
		wc->className = name;
	}

	return wc;

}



WindowClassPtr WindowClass::getDefaultWindowClass() {
	return defaultClass;
}



bool WindowClass::isValid() const {
	return !className.empty();
}



const std::string& WindowClass::getName() const {
	return className;
}