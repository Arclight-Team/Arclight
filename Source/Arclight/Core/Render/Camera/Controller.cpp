/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 Controller.cpp
 */

#include "Controller.hpp"
#include "Input/InputContext.hpp"



void CameraController::enableInput() {

	if (!inputContext)
		return;

	inputContext->enable();

}

void CameraController::disableInput() {

	if (!inputContext)
		return;

	inputContext->disable();

}

void CameraController::setInputMode(CameraInputMode mode) {

	if (!inputContext)
		return;

	switch (mode) {

	case CameraInputMode::Default:
	case CameraInputMode::Keyboard:
	case CameraInputMode::Mouse:

		inputContext->enable();
		inputContext->switchState(static_cast<u32>(mode));

		break;

	case CameraInputMode::None:

		inputContext->disable();

		break;

	default:
		arc_force_assert("Invalid CameraInputMode provided");
		break;

	}

}
