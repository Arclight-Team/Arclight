/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Mouse.hpp
 */

#pragma once

#include "Key.hpp"
#include "Math/Vector.hpp"
#include "Util/BitmaskEnum.hpp"

#include <memory>



class InputSystem;

class Mouse {

public:

	enum class DeviceFlags {
		None = 0x0,
		Capture = 0x1
	};

	ARC_CREATE_MEMBER_BITMASK_ENUM(DeviceFlags)

	Mouse();
	~Mouse();

	bool initialize(const InputSystem& input, bool trap, DeviceFlags flags = DeviceFlags::None);
	void destroy();

	void dispatchInput(InputSystem& input, void* nativePtr);

	void setTrapped(bool trap);
	bool isTrapped() const;

	Vec2i getCursorPosition(bool screenPos = false) const;
	KeyState getButtonState(Key physicalKey) const;

private:

	constexpr static Key PhysicalKeys[] = { PhysicalKey::MouseButton1, PhysicalKey::MouseButton2, PhysicalKey::MouseButton3, PhysicalKey::MouseButton4, PhysicalKey::MouseButton5 };
	constexpr static Key VirtualKeys[]  = { VirtualKey::MouseButton1,  VirtualKey::MouseButton2,  VirtualKey::MouseButton3,  VirtualKey::MouseButton4,  VirtualKey::MouseButton5  };

	void enableHardwareMotion();
	void disableHardwareMotion();
	bool isHardwareMotionEnabled() const;

	std::weak_ptr<class WindowHandle> windowHandle;
	DeviceFlags devFlags;

	bool trapped;
	bool hwMotion;
	bool cursorEntered;

};
