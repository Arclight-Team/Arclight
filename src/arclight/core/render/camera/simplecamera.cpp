/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 simplecamera.cpp
 */

#include "simplecamera.hpp"
#include "input/inputcontext.hpp"
#include "input/inputhandler.hpp"



#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

#include "render/gui/arcgui.hpp"

#endif


enum class CameraAction
{
	MoveLeft,
	MoveRight,
	MoveForward,
	MoveBackward,
	MoveUp,
	MoveDown,

	SpeedModifierOn,
	SpeedModifierOff,
	SpeedUp,
	SlowDown,

	GrabStart,
	GrabMove,
};



void SimpleCamera::update() {

	if (movement != Vec3i(0, 0, 0)) {
		move(movement * currentSpeed);
		movement = Vec3i(0);
	}

	if (rotation != Vec3i(0, 0, 0)) {
		rotate(rotation.x * defaultRotationSpeed, rotation.y * defaultRotationSpeed);
		rotation = Vec3i(0);
	}

}



void SimpleCamera::setupInputContext(InputContext& context, u32 firstAction, u32 contextState) {

	firstActionID = firstAction;

	context.addAction(firstAction++, config.moveLeft, true);
	context.addAction(firstAction++, config.moveRight, true);
	context.addAction(firstAction++, config.moveForward, true);
	context.addAction(firstAction++, config.moveBackward, true);
	context.addAction(firstAction++, config.moveUp, true);
	context.addAction(firstAction++, config.moveDown, true);

	context.addAction(firstAction++, KeyTrigger(config.speedModifier, KeyState::Pressed));
	context.addAction(firstAction++, KeyTrigger(config.speedModifier, KeyState::Released));
	context.addAction(firstAction++, config.speedUp);
	context.addAction(firstAction++, config.slowDown);

	context.addAction(firstAction++, config.drag);
	context.addAction(firstAction++, config.drag, true);

	for (u32 i = firstActionID; i < firstAction; i++) {
		context.registerAction(contextState, i);
	}

}

void SimpleCamera::setupInputHandler(InputHandler& handler) {

	handler.setActionListener([this](KeyAction action) {
		actionListener(action);
		return true;
	});

	handler.setCoActionListener([this](KeyAction action, double) {
		actionListener(action);
		return true;
	});

	handler.setCursorListener([this](double x, double y) {
		cursorListener(x, y);
		return true;
	});

}



bool SimpleCamera::actionListener(KeyAction action) {

	float speedMin = defaultMovementSpeed / 8.0f;
	float speedMax = defaultMovementSpeed * 8.0f;
	float speedInc = defaultMovementSpeed / 10.0f;

	switch (static_cast<CameraAction>(action - firstActionID)) {

	case CameraAction::MoveLeft:
		if (inputBusy())
			return false;

		movement.x -= 1;
		break;

	case CameraAction::MoveRight:
		if (inputBusy())
			return false;

		movement.x += 1;
		break;

	case CameraAction::MoveForward:
		if (inputBusy())
			return false;

		movement.z += 1;
		break;

	case CameraAction::MoveBackward:
		if (inputBusy())
			return false;

		movement.z -= 1;
		break;

	case CameraAction::MoveUp:
		if (inputBusy())
			return false;

		movement.y += 1;
		break;

	case CameraAction::MoveDown:
		if (inputBusy())
			return false;

		movement.y -= 1;
		break;

	case CameraAction::SpeedModifierOn:
		currentSpeed = speedMax;
		break;

	case CameraAction::SpeedModifierOff:
		currentSpeed = defaultMovementSpeed;
		break;

	case CameraAction::SpeedUp:
		currentSpeed += speedInc;
		currentSpeed = Math::clamp(currentSpeed, speedMin, speedMax);
		break;

	case CameraAction::SlowDown:
		currentSpeed -= speedInc;
		currentSpeed = Math::clamp(currentSpeed, speedMin, speedMax);
		break;

	case CameraAction::GrabStart:
		if (inputBusy())
			return false;

		mouseGrab = mouse;
		break;

	case CameraAction::GrabMove:
	{
		if (inputBusy())
			return false;

		Vec2f dif = mouse - mouseGrab;
		mouseGrab = mouse;
		dif *= defaultRotationSpeed * 0.092;
		rotate(dif.x, -dif.y);
		break;
	}

	default:
		return false;

	}

	return true;

}

bool SimpleCamera::cursorListener(double x, double y) {

	if (inputBusy())
		return false;

	mouse.x = int(x);
	mouse.y = int(y);

	return true;

}



CameraInputConfig& SimpleCamera::getConfig() {
	return config;
}

const CameraInputConfig& SimpleCamera::getConfig() const {
	return config;
}

void SimpleCamera::setConfig(const CameraInputConfig& config) {
	this->config = config;
}



bool SimpleCamera::inputBusy() const {

#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

	return ImGui::GetCurrentContext() && ImGui::IsAnyItemActive();

#else

	return false;

#endif

}
