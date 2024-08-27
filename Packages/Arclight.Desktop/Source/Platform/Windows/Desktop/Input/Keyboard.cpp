/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Keyboard.cpp
 */

#include "Desktop/Input/Keyboard.hpp"
#include "Desktop/Input/InputSystem.hpp"
#include "Desktop/Window/WindowHandle.hpp"
#include "Common/Win32.hpp"
#include "System/Common.hpp"

#include <hidusage.h>



Keyboard::Keyboard() : devFlags(DeviceFlags::None), propagateHeld(false) {}

Keyboard::~Keyboard() {
	destroy();
}



bool Keyboard::initialize(const InputSystem& input, DeviceFlags flags) {

	if (!input.connected()) {
		return false;
	}

	auto handle = input.getWindowHandle();

	if (!handle) {
		return false;
	}

	DWORD winFlags = RIDEV_NOLEGACY;

	if (bool(flags & DeviceFlags::Capture)) {
		winFlags |= RIDEV_INPUTSINK;
	}

	if (!bool(flags & DeviceFlags::OverrideCommands)) {
		winFlags |= RIDEV_APPKEYS;
	}

	RAWINPUTDEVICE device;
	device.usUsagePage = HID_USAGE_PAGE_GENERIC;
	device.usUsage = HID_USAGE_GENERIC_KEYBOARD;
	device.dwFlags = winFlags;
	device.hwndTarget = handle->getHWND();

	if (!RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE))) {
		return false;
	}

	devFlags = flags;
	windowHandle = handle;

	return true;

}



void Keyboard::destroy() {

	if (auto handle = windowHandle.lock()) {

		RAWINPUTDEVICE device;
		device.usUsagePage = HID_USAGE_PAGE_GENERIC;
		device.usUsage = HID_USAGE_GENERIC_KEYBOARD;
		device.dwFlags = RIDEV_REMOVE;
		device.hwndTarget = nullptr;

		RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));

		windowHandle.reset();
		devFlags = DeviceFlags::None;

	}

}



void Keyboard::dispatchInput(InputSystem& input, const KeyContext& keyContext, void* nativePtr) {

	const RAWKEYBOARD& keyboard = *static_cast<RAWKEYBOARD*>(nativePtr);

	bool extendedKey = keyboard.Flags & (RI_KEY_E0 | RI_KEY_E1);
	Key scancode = keyboard.MakeCode | (keyboard.MakeCode ? Key(extendedKey) << 8 : 0);
	Key vkCode = keyboard.VKey;
	KeyState state = keyboard.Flags & RI_KEY_BREAK ? KeyState::Released : KeyState::Pressed;

	Key virtualKey = 0;

	// Bad key or too many keys pressed at once
	if (keyboard.MakeCode == KEYBOARD_OVERRUN_MAKE_CODE) {
		return;
	}

	if (keyboard.Flags & RI_KEY_E1) {

		if (vkCode == VK_PAUSE) {

			// Pause key
			scancode = 0x45;

		} else {

			scancode = MapVirtualKeyW(vkCode, MAPVK_VK_TO_VSC);

		}

		virtualKey = KeyConversion::winVKToVirtualKey(vkCode);

	} else {

		if (extendedKey) {

			switch (vkCode) {

				case VK_SHIFT:		virtualKey = KeyConversion::winVKToVirtualKey(MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK_EX));	break;
				case VK_CONTROL:	virtualKey = VirtualKey::RightControl;	break;
				case VK_MENU:		virtualKey = VirtualKey::RightAlt;		break;
				case VK_RETURN:		virtualKey = VirtualKey::NumpadEnter;	break;
				default:			virtualKey = KeyConversion::winVKToVirtualKey(vkCode);	break;

			}

		} else {

			switch (vkCode) {

				case VK_SHIFT:		virtualKey = KeyConversion::winVKToVirtualKey(MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK_EX));	break;
				case VK_CONTROL:	virtualKey = VirtualKey::LeftControl;	break;
				case VK_MENU:		virtualKey = VirtualKey::LeftAlt;		break;
				case VK_INSERT:		virtualKey = VirtualKey::NumpadZero;	break;
				case VK_END:		virtualKey = VirtualKey::NumpadOne;		break;
				case VK_DOWN:		virtualKey = VirtualKey::NumpadTwo;		break;
				case VK_NEXT:		virtualKey = VirtualKey::NumpadThree;	break;
				case VK_LEFT:		virtualKey = VirtualKey::NumpadFour;	break;
				case VK_CLEAR:		virtualKey = VirtualKey::NumpadFive;	break;
				case VK_RIGHT:		virtualKey = VirtualKey::NumpadSix;		break;
				case VK_HOME:		virtualKey = VirtualKey::NumpadSeven;	break;
				case VK_UP:			virtualKey = VirtualKey::NumpadEight;	break;
				case VK_PRIOR:		virtualKey = VirtualKey::NumpadNine;	break;
				case VK_DELETE:		virtualKey = VirtualKey::NumpadDecimal;	break;
				case VK_NUMLOCK:	scancode = 0x145;
				default:			virtualKey = KeyConversion::winVKToVirtualKey(vkCode);	break;

			}

		}

	}

	if (keyboard.VKey == 255 || (scancode == 0x138 && keyboard.VKey == VK_CONTROL)) {

		// Discard unknown key / LControl of AltGr sequence
		return;

	}

	Key physicalKey = KeyConversion::scancodeToPhysicalKey(scancode);
	KeyState prevState = keyContext.getKeyMap(true)[physicalKey] ? KeyState::Pressed : KeyState::Released;

	if (state == KeyState::Pressed) {

		if (prevState == KeyState::Released) {
			input.onKeyEvent(KeyEvent(physicalKey, virtualKey, KeyState::Pressed));
		} else if (isHeldEventEnabled()) {
			input.onKeyEvent(KeyEvent(physicalKey, virtualKey, KeyState::Held));
		}

	} else if (prevState != KeyState::Released) {

		input.onKeyEvent(KeyEvent(physicalKey, virtualKey, KeyState::Released));

	}

}



void Keyboard::enableHeldEvent() {
	propagateHeld = true;
}



void Keyboard::disableHeldEvent() {
	propagateHeld = false;
}



bool Keyboard::isHeldEventEnabled() const {
	return propagateHeld;
}



Scancode Keyboard::getScancode(Key physicalKey) {
	return KeyConversion::physicalKeyToScancode(physicalKey);
}



Key Keyboard::getPhysicalKey(Scancode scancode) {
	return KeyConversion::scancodeToPhysicalKey(scancode);
}



std::string Keyboard::getKeyNameFromScancode(Scancode code) {

	// In a later iteration, provide custom key names
	std::wstring str(64, L'\0');
	SizeT size = GetKeyNameTextW(code << 16, str.data(), str.size());
	str.resize(size);

	return System::String::toUTF8(str);

}