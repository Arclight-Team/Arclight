/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputDevice.hpp
 */

#pragma once

#include "Common/Types.hpp"

#include <string>
#include <vector>



struct InputDeviceInfo {

	std::string productName;
	std::string manufacturer;
	std::string serialNumber;

};

struct KeyboardDeviceInfo : InputDeviceInfo {

	u32 functionKeys {};
	u32 leds {};
	u32 totalKeys {};

};

struct MouseDeviceInfo : InputDeviceInfo {

	bool hasWheel {};
	bool hasHorizontalWheel {};
	u32 buttons {};
	u32 sampleRate {};

};

namespace InputDevice {

	std::vector<KeyboardDeviceInfo> enumerateKeyboardDevices();
	std::vector<MouseDeviceInfo> enumerateMouseDevices();
	std::vector<InputDeviceInfo> enumerateInputDevices();

}