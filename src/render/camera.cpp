#include "camera.h"



Camera::Camera() : Camera(Vec3f(1, 0, 0)) {}

Camera::Camera(const Vec3f& pos) : Camera(pos, 0, 0) {};

Camera::Camera(const Vec3f& pos, double angleH, double angleV) : oldPosition(), oldFront() {
	setPosition(pos);
	setRotation(angleH, angleV);
};



void Camera::update(double posBlendInc, double dirBlendInc) {

	if (blendPosition < 1) {

		blendPosition += posBlendInc;

		if (blendPosition > 1) {
			blendPosition = 1;
		}

	}

	if (blendRotation < 1) {

		blendRotation += dirBlendInc;

		if (blendRotation > 1) {
			blendRotation = 1;
		}

	}

}



void Camera::move(const Vec3f& movement) {

	oldPosition = getPosition();
	newPosition = oldPosition;

	Vec3f tmpFront = getDirection();
	Vec3f right = Vec3f::normalize(tmpFront.cross(Vec3f(0, 1, 0)));

	newPosition += right * movement.x;
	newPosition += tmpFront * movement.z;

	blendPosition = 0;

}



void Camera::rotate(double angleH, double angleV) {

	yaw += angleH;
	pitch += angleV;
	clampPitch();

	oldFront = getDirection();
	newFront = Vec3f::normalize(Vec3f(Math::cos(yaw) * Math::cos(pitch), Math::sin(pitch), Math::sin(yaw) * Math::cos(pitch)));

	blendRotation = 0;

}



void Camera::setPosition(const Vec3f& pos) {
	
	newPosition = pos;
	blendPosition = 1;

}



void Camera::setRotation(double angleH, double angleV) {

	yaw = angleH;
	pitch = angleV;
	clampPitch();

	newFront = Vec3f::normalize(Vec3f(Math::cos(yaw) * Math::cos(pitch), Math::sin(pitch), Math::sin(yaw) * Math::cos(pitch)));

	blendRotation = 1;

}



Vec3f Camera::getPosition() const {

	return Vec3f(Math::lerp(oldPosition.x, newPosition.x, blendPosition),
				 Math::lerp(oldPosition.y, newPosition.y, blendPosition),
				 Math::lerp(oldPosition.z, newPosition.z, blendPosition));

}



Vec3f Camera::getDirection() const {

	return Vec3f(Math::lerp(oldFront.x, newFront.x, blendRotation),
				 Math::lerp(oldFront.y, newFront.y, blendRotation),
				 Math::lerp(oldFront.z, newFront.z, blendRotation));

}



void Camera::clampPitch() {
	pitch = Math::clamp(pitch, Math::toRadians(-89.99), Math::toRadians(89.99));
}