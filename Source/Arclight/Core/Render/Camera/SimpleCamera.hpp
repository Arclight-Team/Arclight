/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 SimpleCamera.hpp
 */

#pragma once

#include "Camera.hpp"
#include "Controller.hpp"



class InputContext;
class InputHandler;

struct CameraInputConfig
{
	KeyTrigger moveLeft;
	KeyTrigger moveRight;
	KeyTrigger moveForward;
	KeyTrigger moveBackward;

	KeyTrigger moveUp;
	KeyTrigger moveDown;

	KeyTrigger speedModifier;	// hold to instantly speed up, release to instantly slow down
	KeyTrigger speedUp;			// press to speed up by a factor
	KeyTrigger slowDown;		// press to slow down by a factor

	KeyTrigger drag;
};


class SimpleCamera : public Camera, public CameraController
{
public:
	
	static constexpr float DefaultMovementSpeed = 0.08;
	static constexpr float DefaultRotationSpeed = 0.043;


	constexpr void setSpeed(float movement = DefaultMovementSpeed, float rotation = DefaultRotationSpeed) {
		currentSpeed = defaultMovementSpeed = movement;
		defaultRotationSpeed = rotation;
	}

	void update();

	void setupInputContext(InputContext& context, u32 firstAction, u32 contextState = 0);
	void setupInputHandler(InputHandler& handler);
		
	bool actionListener(KeyAction action);
	bool cursorListener(double x, double y);

	CameraInputConfig& getConfig();
	const CameraInputConfig& getConfig() const;
	void setConfig(const CameraInputConfig& config);

private:

	bool inputBusy() const;

	constexpr static CameraInputConfig defaultConfig =
	{
		KeyTrigger({ KeyCode::A }),
		KeyTrigger({ KeyCode::D }), 
		KeyTrigger({ KeyCode::W }), 
		KeyTrigger({ KeyCode::S }),
	
		KeyTrigger({ KeyCode::E }),
		KeyTrigger({ KeyCode::Q }),
	
		KeyTrigger({ KeyCode::LeftControl }),
		KeyTrigger({ KeyCode::X }),
		KeyTrigger({ KeyCode::Z }),

		KeyTrigger({ MouseCode::Left }),
	};

	CameraInputConfig config = defaultConfig;
	u32 firstActionID;

	Vec2i mouse;
	Vec2i mouseGrab;
	Vec3i movement;
	Vec3i rotation;

	float defaultMovementSpeed = DefaultMovementSpeed;
	float defaultRotationSpeed = DefaultRotationSpeed;

	float currentSpeed = DefaultMovementSpeed;

};