/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 camera.cpp
 */

#include "camera.hpp"



Camera::Camera() : Camera(Vec3f(1, 0, 0)) {}

Camera::Camera(const Vec3f& pos) : Camera(pos, 0, 0) {};

Camera::Camera(const Vec3f& pos, double angleH, double angleV) {
	setPosition(pos);
	setRotation(angleH, angleV);
};



void Camera::move(const Vec3f& movement) {

	Vec3f front = getDirection();
	Vec3f right = Vec3f::normalize(front.cross(Vec3f(0, 1, 0)));

	position += right * movement.x + Vec3f(0, 1, 0) * movement.y + front * movement.z;

}



void Camera::rotate(double angleH, double angleV) {

	yaw += angleH;
	pitch += angleV;
	clampPitch();

	direction = Vec3f::normalize(Vec3f(Math::cos(yaw) * Math::cos(pitch), Math::sin(pitch), Math::sin(yaw) * Math::cos(pitch)));

}



void Camera::setPosition(const Vec3f& pos) {
	position = pos;
}



void Camera::setRotation(double angleH, double angleV) {

	yaw = angleH;
	pitch = angleV;
	clampPitch();

	direction = Vec3f::normalize(
		Vec3f
		(
			Math::cos(yaw) * Math::cos(pitch),
			Math::sin(pitch), 
			Math::sin(yaw) * Math::cos(pitch)
		)
	);

}



Vec3f Camera::getPosition() const {
	return position;
}



Vec3f Camera::getDirection() const {
	return direction;
}



void Camera::clampPitch() {
	pitch = Math::clamp(pitch, Math::toRadians(-89.99), Math::toRadians(89.99));
}