/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 arcballcamera.hpp
 */

#pragma once

#include "camera.hpp"
#include "controller.hpp"



class InputContext;
class InputHandler;

struct ArcballCameraInputConfig
{
	KeyTrigger moveLeft;
	KeyTrigger moveRight;
	KeyTrigger moveUp;
	KeyTrigger moveDown;

	KeyTrigger zoomIn;
	KeyTrigger zoomOut;

	KeyTrigger speedModifier;	// hold to instantly speed up, release to instantly slow down
	KeyTrigger speedUp;			// press to speed up by a factor
	KeyTrigger slowDown;		// press to slow down by a factor

	KeyTrigger drag;
};



class ArcballCamera : public CameraController
{
public:

	static constexpr float DefaultSpeed = 0.04;

	ArcballCamera();
	ArcballCamera(double z, double yaw, double pitch);

	void move(double z);
	void rotate(double yaw, double pitch);

	void setDistance(double distance);
	void setRotation(double yaw, double pitch);

	constexpr void setSpeed(float speed = DefaultSpeed) {
		currentSpeed = defaultSpeed = speed;
	}

	void update();

	void setupInputContext(InputContext& context, u32 firstAction);
	void setupInputHandler(InputHandler& handler);
		
	bool actionListener(KeyAction action);
	bool cursorListener(double x, double y);
	bool scrollListener(double y);

	ArcballCameraInputConfig& getConfig();
	const ArcballCameraInputConfig& getConfig() const;
	void setConfig(const ArcballCameraInputConfig& config);

	constexpr Vec3f getPosition() {
		return position * distance;
	}

	constexpr float getDistance() {
		return distance;
	}

	constexpr Vec3f getTarget() {
		return {};
	}

private:

	bool inputBusy() const;

	constexpr static ArcballCameraInputConfig defaultConfig =
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

	ArcballCameraInputConfig config = defaultConfig;
	u32 firstActionID;

	Vec2i mouse;
	Vec2i mouseGrab;

	Vec3f position;
	double distance;
	double pitch;
	double yaw;

	i32 movementZ{}; // distance
	i32 rotationX{}; // pitch
	i32 rotationY{}; // yaw

	float defaultSpeed = DefaultSpeed;
	float currentSpeed = DefaultSpeed;

};