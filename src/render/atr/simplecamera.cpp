#include "simplecamera.h"
#include "input/inputcontext.h"
#include "input/inputhandler.h"

#if defined(ARC_USE_IMGUI) && ARC_USE_IMGUI

#include "render/gui/arcgui.h"

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
		move(movement * movementSpeed);
		movement = Vec3i(0);
	}

	if (inputBusy())
		return;

	if (rotation != Vec3i(0, 0, 0)) {
		rotate(rotation.x * rotationSpeed, rotation.y * rotationSpeed);
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

	// TODO: add them as class members (getters/setters)
	constexpr float speedMin = 0.02f;
	constexpr float speedDef = 0.08f;
	constexpr float speedMax = 0.50f;
	constexpr float speedInc = 0.02f;

	switch (static_cast<CameraAction>(action - firstActionID)) {

	case CameraAction::MoveLeft:
		movement.x -= 1;
		break;

	case CameraAction::MoveRight:
		movement.x += 1;
		break;

	case CameraAction::MoveForward:
		movement.z += 1;
		break;

	case CameraAction::MoveBackward:
		movement.z -= 1;
		break;

	case CameraAction::MoveUp:
		movement.y += 1;
		break;

	case CameraAction::MoveDown:
		movement.y -= 1;
		break;

	case CameraAction::SpeedModifierOn:
		movementSpeed = speedMax;
		break;

	case CameraAction::SpeedModifierOff:
		movementSpeed = speedDef;
		break;

	case CameraAction::SpeedUp:
		movementSpeed += speedInc;
		movementSpeed = Math::clamp(movementSpeed, speedMin, speedMax);
		break;

	case CameraAction::SlowDown:
		movementSpeed -= speedInc;
		movementSpeed = Math::clamp(movementSpeed, speedMin, speedMax);
		break;

	case CameraAction::GrabStart:
	{
		if (inputBusy())
			break;

		mouseGrab = mouse;
		break;
	}

	case CameraAction::GrabMove:
	{
		if (inputBusy())
			break;

		Vec2f dif = mouse - mouseGrab;
		mouseGrab = mouse;
		dif *= rotationSpeed * 0.092;
		rotate(dif.x, -dif.y);
		break;
	}

	default:
		return false;

	}

	return true;

}

bool SimpleCamera::cursorListener(double x, double y) {

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
