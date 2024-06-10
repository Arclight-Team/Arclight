/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputDevice.cpp
 */

#include "Desktop/Input/InputDevice.hpp"
#include "Common/Win32.hpp"
#include "Math/Math.hpp"
#include "Util/Range.hpp"
#include "System/Common.hpp"

#include <vector>

#include "hidsdi.h"
#include "ntddmou.h"



static std::string readString(SizeT initialSize, const std::function<bool(std::wstring&, SizeT)>& callback) {

	std::wstring wstr(initialSize, L'\0');

	while (true) {

		if (callback(wstr, Math::min(wstr.size() + 1, 4093) * sizeof(wchar_t))) {
			return System::String::toUTF8(wstr.data());
		}

		if (System::getSystemError() != ERROR_INVALID_USER_BUFFER) {
			return "";
		}

		if (wstr.size() >= 4096) {
			return "";
		}

		wstr.resize(wstr.size() * 2);

	}

}



static std::vector<RAWINPUTDEVICELIST> enumerateRawDevices() {

	UINT deviceCount = 0;
	std::vector<RAWINPUTDEVICELIST> devices;

	for (u32 i : Range(10)) {

		if (GetRawInputDeviceList(nullptr, &deviceCount, sizeof(RAWINPUTDEVICELIST)) != 0 || deviceCount == 0) {
			return {};
		}

		devices.resize(deviceCount);
		deviceCount = GetRawInputDeviceList(devices.data(), &deviceCount, sizeof(RAWINPUTDEVICELIST));

		if (deviceCount == UINT(-1)) {

			if (System::getSystemError() != ERROR_INSUFFICIENT_BUFFER) {
				return {};
			}

			continue;

		}

		break;

	}

	if (deviceCount == UINT(-1)) {
		return {};
	}

	return devices;

}




template<class T>
std::vector<T> enumerateDevices() {

	u32 deviceType = CC::Equal<T, KeyboardDeviceInfo> ? RIM_TYPEKEYBOARD : (CC::Equal<T, MouseDeviceInfo> ? RIM_TYPEMOUSE : RIM_TYPEHID);

	std::vector<RAWINPUTDEVICELIST> devices = enumerateRawDevices();
	std::vector<T> deviceInfos;

	for (const RAWINPUTDEVICELIST& device : devices) {

		if (device.dwType == deviceType) {

			T info;

			UINT bufferSize = 0;
			GetRawInputDeviceInfoW(device.hDevice, RIDI_DEVICENAME, nullptr, &bufferSize);

			std::wstring wFilename(bufferSize, '\0');
			UINT resultSize = GetRawInputDeviceInfoW(device.hDevice, RIDI_DEVICENAME, wFilename.data(), &bufferSize);

			if (resultSize > 0) {

				wFilename.resize(resultSize - 1);
				HANDLE hidFile = CreateFileW(wFilename.data(), 0, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

				if (hidFile != INVALID_HANDLE_VALUE) {

					info.productName = readString(128, [&](std::wstring& w, SizeT s) { return HidD_GetProductString(hidFile, w.data(), s); });
					info.manufacturer = readString(64, [&](std::wstring& w, SizeT s) { return HidD_GetManufacturerString(hidFile, w.data(), s); });
					info.serialNumber = readString(64, [&](std::wstring& w, SizeT s) { return HidD_GetSerialNumberString(hidFile, w.data(), s); });

				} else {

					// Ignore non-HID devices
					// This includes the default virtual RID device
					continue;

				}

			} else {

				continue;

			}

			if constexpr (!CC::Equal<T, InputDeviceInfo>) {

				bufferSize = sizeof(RID_DEVICE_INFO);

				RID_DEVICE_INFO devInfo;
				devInfo.cbSize = bufferSize;

				if (GetRawInputDeviceInfoW(device.hDevice, RIDI_DEVICEINFO, &devInfo, &bufferSize) > 0) {

					if (devInfo.dwType != deviceType) {
						continue;
					}

					if constexpr (CC::Equal<T, KeyboardDeviceInfo>) {

						const RID_DEVICE_INFO_KEYBOARD& devKeyboardInfo = devInfo.keyboard;

						info.functionKeys = devKeyboardInfo.dwNumberOfFunctionKeys;
						info.leds = devKeyboardInfo.dwNumberOfIndicators;
						info.totalKeys = devKeyboardInfo.dwNumberOfKeysTotal;

					} else if constexpr (CC::Equal<T, MouseDeviceInfo>) {

						const RID_DEVICE_INFO_MOUSE& devMouseInfo = devInfo.mouse;

						info.hasWheel = devMouseInfo.dwId & WHEELMOUSE_HID_HARDWARE;
						info.hasHorizontalWheel = devMouseInfo.fHasHorizontalWheel;
						info.buttons = devMouseInfo.dwNumberOfButtons;
						info.sampleRate = devMouseInfo.dwSampleRate;

					}

				} else {

					continue;

				}

			}

			deviceInfos.emplace_back(info);

		}

	}

	return deviceInfos;

}



std::vector<KeyboardDeviceInfo> InputDevice::enumerateKeyboardDevices() {
	return enumerateDevices<KeyboardDeviceInfo>();
}

std::vector<MouseDeviceInfo> InputDevice::enumerateMouseDevices() {
	return enumerateDevices<MouseDeviceInfo>();
}

std::vector<InputDeviceInfo> InputDevice::enumerateHIDInputDevices() {
	return enumerateDevices<InputDeviceInfo>();
}