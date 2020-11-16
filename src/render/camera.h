#pragma once

#include "util/vector.h"


class Camera {

public:

	Camera();
	explicit Camera(const Vec3f& pos);
	Camera(const Vec3f& pos, double angleH, double angleV);


	void update(double posBlendInc, double dirBlendInc);

	void move(const Vec3f& movement);
	void rotate(double angleH, double angleV);

	void setPosition(const Vec3f& pos);
	void setRotation(double angleH, double angleV);

	Vec3f getPosition() const;
	Vec3f getDirection() const;

private:

	void clampPitch();
	
	Vec3f newPosition;
	Vec3f oldPosition;
	Vec3f newFront;
	Vec3f oldFront;
	double yaw;
	double pitch;
	double blendPosition;
	double blendRotation;

};