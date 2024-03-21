/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Controller.hpp
 */

#pragma once

#include "Common/Types.hpp"
#include "Input/KeyTrigger.hpp"
#include "Input/KeyDefs.hpp"


class InputContext;


enum class CameraInputMode
{
	Default,
	Keyboard,
	Mouse,
	None
};


class CameraController
{
public:

	void enableInput();
	void disableInput();
	void setInputMode(CameraInputMode mode);

	inline void resetInputMode() {
		setInputMode(CameraInputMode::Default);
	}

protected:

	InputContext* inputContext{};

};
