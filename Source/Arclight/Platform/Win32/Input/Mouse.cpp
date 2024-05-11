/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Mouse.cpp
 */

#include "Mouse.hpp"
#include "InputSystem.hpp"
#include "Window/WindowHandle.hpp"
#include "Common/Win32.hpp"
#include "Math/Shapes/Rectangle.hpp"

#include <hidusage.h>
#include <windowsx.h>



Mouse::Mouse() : devFlags(DeviceFlags::None), trapped(false), hwMotion(false), cursorEntered(false) {
	keyStates.fill(KeyState::Released);
}

Mouse::~Mouse() {
	destroy();
}



bool Mouse::initialize(const InputSystem& input, bool trap, DeviceFlags flags) {

	if (!input.connected()) {
		return false;
	}

	auto handle = input.getWindowHandle();

	if (!handle) {
		return false;
	}

	devFlags = flags;
	windowHandle = handle;
	keyStates.fill(KeyState::Released);

	setTrapped(trap);

	return true;

}



void Mouse::destroy() {

	if (auto handle = windowHandle.lock()) {

		setTrapped(false);

		windowHandle.reset();
		devFlags = DeviceFlags::None;
		keyStates.fill(KeyState::Released);

	}

}



void Mouse::dispatchInput(InputSystem& input, void* nativePtr) {

	if (hwMotion) {

		const RAWMOUSE& mouse = *static_cast<RAWMOUSE*>(nativePtr);

		if (mouse.usFlags & MOUSE_MOVE_ABSOLUTE) {

			RectL rect;

			if (mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) {

				rect.x = GetSystemMetrics(SM_XVIRTUALSCREEN);
				rect.y = GetSystemMetrics(SM_YVIRTUALSCREEN);
				rect.w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
				rect.h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

			} else {

				rect.x = 0;
				rect.y = 0;
				rect.w = GetSystemMetrics(SM_CXSCREEN);
				rect.h = GetSystemMetrics(SM_CYSCREEN);

			}

			Vec2d absolute(mouse.lLastX * rect.w / 65535 + rect.x, mouse.lLastY * rect.h / 65535 + rect.y);
			input.onCursorEvent(CursorEvent(absolute.x, absolute.y, false));

		} else if (mouse.lLastX != 0 || mouse.lLastY != 0) {

			i32 relativeX = mouse.lLastX;
			i32 relativeY = mouse.lLastY;
			input.onCursorEvent(CursorEvent(relativeX, relativeY, true));

		}

		if (mouse.usButtonFlags & (RI_MOUSE_WHEEL | RI_MOUSE_HWHEEL) && mouse.usButtonData) {

			SHORT scroll = SHORT(mouse.usButtonData);
			double delta = scroll / double(WHEEL_DELTA);
			input.onScrollEvent(mouse.usButtonFlags & RI_MOUSE_WHEEL ? ScrollEvent(0, delta) : ScrollEvent(delta, 0));

		}

		constexpr u32 buttonAction = 	RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_1_UP |
										RI_MOUSE_BUTTON_2_DOWN | RI_MOUSE_BUTTON_2_UP |
										RI_MOUSE_BUTTON_3_DOWN | RI_MOUSE_BUTTON_3_UP |
										RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_4_UP |
										RI_MOUSE_BUTTON_5_DOWN | RI_MOUSE_BUTTON_5_UP;

		if (mouse.usButtonFlags & buttonAction) {

			constexpr u32 buttonMask[] = {
				RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_1_UP,
				RI_MOUSE_BUTTON_2_DOWN | RI_MOUSE_BUTTON_2_UP,
				RI_MOUSE_BUTTON_3_DOWN | RI_MOUSE_BUTTON_3_UP,
				RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_4_UP,
				RI_MOUSE_BUTTON_5_DOWN | RI_MOUSE_BUTTON_5_UP
			};

			KeyState state = mouse.usButtonFlags & (RI_MOUSE_BUTTON_1_DOWN | RI_MOUSE_BUTTON_2_DOWN | RI_MOUSE_BUTTON_3_DOWN | RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_5_DOWN) ? KeyState::Pressed : KeyState::Released;

			for (u32 i : Range(5)) {

				if (mouse.usButtonFlags & buttonMask[i]) {

					keyStates[i] = state;
					input.onKeyEvent(KeyEvent(PhysicalKeys[i], VirtualKeys[i], state));

				}

			}

		}

	} else {

		MSG& message = *static_cast<MSG*>(nativePtr);
		UINT msgType = message.message;

		switch (msgType) {

			case WM_LBUTTONDOWN:	keyStates[0] = KeyState::Pressed; 	input.onKeyEvent(KeyEvent(PhysicalKey::MouseLeft, VirtualKey::MouseLeft, KeyState::Pressed));		break;
			case WM_LBUTTONUP:		keyStates[0] = KeyState::Released; 	input.onKeyEvent(KeyEvent(PhysicalKey::MouseLeft, VirtualKey::MouseLeft, KeyState::Released));		break;
			case WM_MBUTTONDOWN:	keyStates[2] = KeyState::Pressed; 	input.onKeyEvent(KeyEvent(PhysicalKey::MouseMiddle, VirtualKey::MouseMiddle, KeyState::Pressed));	break;
			case WM_MBUTTONUP:		keyStates[2] = KeyState::Released; 	input.onKeyEvent(KeyEvent(PhysicalKey::MouseMiddle, VirtualKey::MouseMiddle, KeyState::Released));	break;
			case WM_RBUTTONDOWN:	keyStates[1] = KeyState::Pressed; 	input.onKeyEvent(KeyEvent(PhysicalKey::MouseRight, VirtualKey::MouseRight, KeyState::Pressed));		break;
			case WM_RBUTTONUP:		keyStates[1] = KeyState::Released; 	input.onKeyEvent(KeyEvent(PhysicalKey::MouseRight, VirtualKey::MouseRight, KeyState::Released));	break;

			case WM_XBUTTONDOWN:

				if (GET_XBUTTON_WPARAM(message.wParam) == XBUTTON1) {

					keyStates[3] = KeyState::Pressed;
					input.onKeyEvent(KeyEvent(PhysicalKey::MouseButton4, VirtualKey::MouseButton4, KeyState::Pressed));

				} else {

					keyStates[4] = KeyState::Pressed;
					input.onKeyEvent(KeyEvent(PhysicalKey::MouseButton5, VirtualKey::MouseButton5, KeyState::Pressed));

				}

				break;

			case WM_XBUTTONUP:

				if (GET_XBUTTON_WPARAM(message.wParam) == XBUTTON1) {

					keyStates[3] = KeyState::Released;
					input.onKeyEvent(KeyEvent(PhysicalKey::MouseButton4, VirtualKey::MouseButton4, KeyState::Released));

				} else {

					keyStates[4] = KeyState::Released;
					input.onKeyEvent(KeyEvent(PhysicalKey::MouseButton5, VirtualKey::MouseButton5, KeyState::Released));

				}

				break;

			case WM_MOUSEMOVE: {

				if (!cursorEntered) {

					cursorEntered = true;

					TRACKMOUSEEVENT event;
					ZeroMemory(&event, sizeof(event));

					event.cbSize = sizeof(event);
					event.dwFlags = TME_LEAVE;
					event.hwndTrack = windowHandle.lock()->getHWND();

					TrackMouseEvent(&event);
					input.onCursorAreaEvent(CursorAreaEvent(true));

				}

				double x = GET_X_LPARAM(message.lParam);
				double y = GET_Y_LPARAM(message.lParam);
				input.onCursorEvent(CursorEvent(x, y, false));

			} break;

			case WM_MOUSEWHEEL:
			case WM_MOUSEHWHEEL: {

				if (message.wParam == 0) {
					return;
				}

				double delta = GET_WHEEL_DELTA_WPARAM(message.wParam) / double(WHEEL_DELTA);
				input.onScrollEvent(msgType == WM_MOUSEWHEEL ? ScrollEvent(0, delta) : ScrollEvent(delta, 0));

			} break;

			case WM_MOUSELEAVE: {

				cursorEntered = false;
				input.onCursorAreaEvent(CursorAreaEvent(false));

			} break;

			default:
				break;

		}

	}

}



void Mouse::setTrapped(bool trap) {

	if (auto handle = windowHandle.lock()) {

		if (trap) {

			RECT windowRect;
			HWND window = handle->getHWND();

			if (!GetClientRect(window, &windowRect)) {
				return;
			}

			POINT topLeft {
				windowRect.left,
				windowRect.top
			};

			POINT bottomRight {
				windowRect.right,
				windowRect.bottom
			};

			MapWindowPoints(window, nullptr, &topLeft, 1);
			MapWindowPoints(window, nullptr, &bottomRight, 1);

			windowRect.left = topLeft.x;
			windowRect.right = bottomRight.x;
			windowRect.top = topLeft.y;
			windowRect.bottom = bottomRight.y;

			if (!ClipCursor(&windowRect)) {
				return;
			}

			enableHardwareMotion();

			if (!isHardwareMotionEnabled()) {
				return;
			}

		} else {

			if (!ClipCursor(nullptr)) {
				return;
			}

			disableHardwareMotion();

		}

		trapped = trap;

	}

}



bool Mouse::isTrapped() const {
	return trapped;
}



void Mouse::releaseAllKeys(InputSystem& input) {

	for (u32 i : Range(keyStates.size())) {

		KeyState& state = keyStates[i];

		if (state == KeyState::Pressed) {

			state = KeyState::Released;
			input.onKeyEvent(KeyEvent(PhysicalKeys[i], VirtualKeys[i], KeyState::Released));

		}

	}

}



Vec2i Mouse::getCursorPosition(bool screenPos) const {

	if (auto handle = windowHandle.lock()) {

		POINT pos;

		if (!GetCursorPos(&pos)) {
			return {};
		}

		if (!screenPos) {

			if (!ScreenToClient(handle->getHWND(), &pos)) {
				return {};
			}

		}

		return { pos.x, pos.y };

	}

	return {};

}



KeyState Mouse::getButtonState(Key physicalKey) const {

	switch (physicalKey) {

		case PhysicalKey::MouseButton1:	return keyStates[0];
		case PhysicalKey::MouseButton2:	return keyStates[1];
		case PhysicalKey::MouseButton3:	return keyStates[2];
		case PhysicalKey::MouseButton4:	return keyStates[3];
		case PhysicalKey::MouseButton5:	return keyStates[4];
		default:	return KeyState::Released;

	}

}



void Mouse::enableHardwareMotion() {

	if (hwMotion) {
		return;
	}

	if (auto handle = windowHandle.lock()) {

		DWORD winFlags = RIDEV_NOLEGACY;

		if (bool(devFlags & DeviceFlags::Capture)) {
			winFlags |= RIDEV_INPUTSINK;
		}

		RAWINPUTDEVICE device;
		device.usUsagePage = HID_USAGE_PAGE_GENERIC;
		device.usUsage = HID_USAGE_GENERIC_MOUSE;
		device.dwFlags = winFlags;
		device.hwndTarget = handle->getHWND();

		RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));

		hwMotion = true;

	}

}



void Mouse::disableHardwareMotion() {

	if (!hwMotion) {
		return;
	}

	RAWINPUTDEVICE device;
	device.usUsagePage = HID_USAGE_PAGE_GENERIC;
	device.usUsage = HID_USAGE_GENERIC_MOUSE;
	device.dwFlags = RIDEV_REMOVE;
	device.hwndTarget = nullptr;

	if (RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE))) {
		hwMotion = false;
	}

}



bool Mouse::isHardwareMotionEnabled() const {
	return hwMotion;
}