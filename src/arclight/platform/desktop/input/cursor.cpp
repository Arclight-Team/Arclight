/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cursor.cpp
 */

#include "cursor.hpp"
#include "inputsystem.hpp"
#include "window/window.hpp"

#include <GLFW/glfw3.h>



Cursor::Cursor(const InputSystem& system) : system(system) {}



void Cursor::setPosition(const Vec2d& pos) {
	setPosition(pos.x, pos.y);
}



void Cursor::setPosition(double x, double y) {

	auto window = system.getWindowHandle();

	if (window) {
		glfwSetCursorPos(window->handle, x, y);
	}

}



Vec2d Cursor::getPosition() const {

	Vec2d position;

	auto window = system.getWindowHandle();

	if (window) {
		glfwGetCursorPos(window->handle, &position.x, &position.y);
	}

	return position;

}



void Cursor::show() {

	auto window = system.getWindowHandle();

	if (window) {
		glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}



void Cursor::hide() {

	auto window = system.getWindowHandle();

	if (window) {
		glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}

}



void Cursor::free() {

	auto window = system.getWindowHandle();

	if (window) {
		glfwSetInputMode(window->handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

}



void Cursor::enableRawMotion() {

	if (glfwRawMouseMotionSupported()) {

		auto window = system.getWindowHandle();

		if (window) {
			glfwSetInputMode(window->handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

	}

}



void Cursor::disableRawMotion() {

	if (glfwRawMouseMotionSupported()) {

		auto window = system.getWindowHandle();

		if (window) {
			glfwSetInputMode(window->handle, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
		}

	}

}