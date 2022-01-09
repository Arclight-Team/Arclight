/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcballcamera.cpp
 */

#include "arcballcamera.hpp"
#include "input/inputcontext.hpp"
#include "input/inputhandler.hpp"
#include "math/matrix.hpp"

#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

#include "render/gui/arcgui.hpp"

#endif


enum class CameraAction
{
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	ZoomIn,
	ZoomOut,

	SpeedModifierOn,
	SpeedModifierOff,
	SpeedUp,
	SlowDown,

	GrabStart,
	GrabMove,
	Scroll,
};



ArcballCamera::ArcballCamera() : ArcballCamera(1, 0, 0) {}

ArcballCamera::ArcballCamera(double z, double yaw, double pitch) :
	distance(0),
	yaw(0),
	pitch(0)
{
	setDistance(z);
	setRotation(yaw, pitch);
}



void ArcballCamera::move(double z) {
	setDistance(distance + z);
}

void ArcballCamera::rotate(double angleH, double angleV) {
	setRotation(yaw + angleH, pitch + angleV);
}



void ArcballCamera::setDistance(double z) {
	distance = Math::max(z, Math::epsilon);
}

void ArcballCamera::setRotation(double angleH, double angleV) {

	yaw = angleH;
	pitch = Math::clamp(angleV, Math::toRadians(-89.99), Math::toRadians(89.99));

	position.x = Math::cos(yaw) * Math::cos(pitch);
	position.y = Math::sin(pitch);
	position.z = Math::sin(yaw) * Math::cos(pitch);

}



void ArcballCamera::update() {

	if (movementZ) {
		move(movementZ * currentSpeed);
		movementZ = 0;
	}

	if (rotationX || rotationY) {
		rotate(rotationY * currentSpeed, rotationX * currentSpeed);
		rotationX = rotationY = 0;
	}

}



void ArcballCamera::setupInputContext(InputContext& context, u32 firstAction, u32 contextState) {

	firstActionID = firstAction;

	context.addAction(firstAction++, config.moveLeft, true);
	context.addAction(firstAction++, config.moveRight, true);
	context.addAction(firstAction++, config.moveUp, true);
	context.addAction(firstAction++, config.moveDown, true);
	context.addAction(firstAction++, config.zoomIn, true);
	context.addAction(firstAction++, config.zoomOut, true);

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

void ArcballCamera::setupInputHandler(InputHandler& handler) {

	handler.setActionListener([this](KeyAction action) {
		return actionListener(action);
	});

	handler.setCoActionListener([this](KeyAction action, double) {
		return actionListener(action);
	});

	handler.setCursorListener([this](double x, double y) {
		return cursorListener(x, y);
	});

	handler.setScrollListener([this](double x, double y) {
		return scrollListener(y);
	});

}



bool ArcballCamera::actionListener(KeyAction action) {

	float speedMin = defaultSpeed / 8.0f;
	float speedMax = defaultSpeed * 8.0f;
	float speedInc = defaultSpeed / 10.0f;

	switch (static_cast<CameraAction>(action - firstActionID)) {

	case CameraAction::MoveLeft:
		if (inputBusy())
			return false;

		rotationY -= 1;
		break;

	case CameraAction::MoveRight:
		if (inputBusy())
			return false;

		rotationY += 1;
		break;

	case CameraAction::MoveUp:
		if (inputBusy())
			return false;

		rotationX += 1;
		break;

	case CameraAction::MoveDown:
		if (inputBusy())
			return false;

		rotationX -= 1;
		break;

	case CameraAction::ZoomIn:
		if (inputBusy())
			return false;

		movementZ -= 1;
		break;

	case CameraAction::ZoomOut:
		if (inputBusy())
			return false;

		movementZ += 1;
		break;

	case CameraAction::SpeedModifierOn:
		currentSpeed = speedMax;
		break;

	case CameraAction::SpeedModifierOff:
		currentSpeed = defaultSpeed;
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
		dif *= defaultSpeed * 0.092;
		rotate(dif.x, dif.y);
		break;
	}

	default:
		return false;

	}

	return true;

}

bool ArcballCamera::cursorListener(double x, double y) {

	if (inputBusy())
		return false;

	mouse.x = int(x);
	mouse.y = int(y);

	return true;

}

bool ArcballCamera::scrollListener(double y) {

	if (inputBusy())
		return false;

	move(-y * 0.092);

	return true;

}



ArcballCameraInputConfig& ArcballCamera::getConfig() {
	return config;
}

const ArcballCameraInputConfig& ArcballCamera::getConfig() const {
	return config;
}

void ArcballCamera::setConfig(const ArcballCameraInputConfig& config) {
	this->config = config;
}



bool ArcballCamera::inputBusy() const {

#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

	return ImGui::GetCurrentContext() && ImGui::IsAnyItemActive();

#else

	return false;

#endif

}
