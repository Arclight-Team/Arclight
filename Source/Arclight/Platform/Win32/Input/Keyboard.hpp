/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Keyboard.hpp
 */

#pragma once

#include "InputEvent.hpp"
#include "Util/BitmaskEnum.hpp"

#include <memory>
#include <vector>
#include <functional>
#include <string>


class InputSystem;

class Keyboard {

public:

	enum class DeviceFlags {
		None = 0x0,
		Capture = 0x1,
		OverrideCommands = 0x2
	};

	ARC_CREATE_MEMBER_BITMASK_ENUM(DeviceFlags)

	Keyboard();
	~Keyboard();

	bool initialize(const InputSystem& input, DeviceFlags flags = DeviceFlags::None);
	void destroy();

	void dispatchInput(InputSystem& input, void* nativePtr);

	void enableHeldEvent();
	void disableHeldEvent();
	bool isHeldEventEnabled() const;

	void releaseAllKeys(InputSystem& input);

	KeyState getKeyState(Scancode scancode) const;

	static Scancode getScancode(Key physicalKey);
	static Key getPhysicalKey(Scancode scancode);

private:

	static std::string getKeyNameFromScancode(Scancode code);

	std::weak_ptr<class WindowHandle> windowHandle;
	DeviceFlags devFlags;

	std::vector<KeyState> rawKeyStates;
	bool propagateHeld;

};