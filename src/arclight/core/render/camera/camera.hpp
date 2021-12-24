#pragma once

#include "math/vector.hpp"


class Camera {

public:

	Camera();
	explicit Camera(const Vec3f& pos);
	Camera(const Vec3f& pos, double angleH, double angleV);

	void move(const Vec3f& movement);
	void rotate(double angleH, double angleV);

	void setPosition(const Vec3f& pos);
	void setRotation(double angleH, double angleV);

	Vec3f getPosition() const;
	Vec3f getDirection() const;

	Vec3f getTarget() {
		return getPosition() + getDirection();
	}

private:

	void clampPitch();
	
	Vec3f position;
	Vec3f direction;
	double yaw;
	double pitch;

};