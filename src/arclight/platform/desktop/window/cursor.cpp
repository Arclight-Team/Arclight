/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cursor.cpp
 */

#include "cursor.hpp"
#include "window.hpp"
#include "image/image.hpp"
#include "util/bits.hpp"

#include <GLFW/glfw3.h>




Cursor::Cursor(const Window& window) noexcept : window(window), currentID(DefaultCursorID) {}

Cursor::~Cursor() noexcept {
	destroyAllCursors();
}



void Cursor::restoreDefaultCursor() {
	setCurrentCursor(DefaultCursorID);
}



void Cursor::setCurrentCursor(CursorID id) {

	if (isActiveCursor(id)) {
		return;
	}

	if (id == DefaultCursorID) {

		glfwSetCursor(window.windowHandle->handle, nullptr);
		currentID = id;

	} else if (cursors.contains(id)) {

		glfwSetCursor(window.windowHandle->handle, cursors[id]);
		currentID = id;

	} else {

		Log::warn("Cursor", "Failed to find matching cursor with ID = %d", id);

	}

}



bool Cursor::isActiveCursor(CursorID id) const {
	return id == currentID;
}



bool Cursor::setCursor(CursorID id, const Image<Pixel::RGBA8>& image, const Vec2i& hotspot) {

	if (id == DefaultCursorID) {
		return false;
	}

	GLFWimage img { static_cast<i32>(image.getWidth()), static_cast<i32>(image.getHeight()), const_cast<u8*>(Bits::toByteArray(image.getImageBuffer().data())) };

	GLFWcursor* c = glfwCreateCursor(&img, hotspot.x, hotspot.y);

	if (!c) {

		Log::error("Cursor", "Failed to create image cursor");
		return false;

	}

	cursors[id] = c;
	return true;

}



bool Cursor::setCursor(CursorID id, StandardCursor cursor) {

	if (id == DefaultCursorID) {
		return false;
	}

	i32 v;

	switch (cursor) {

		default:
		case StandardCursor::Arrow:
			v = GLFW_ARROW_CURSOR;
			break;

		case StandardCursor::Text:
			v = GLFW_IBEAM_CURSOR;
			break;

		case StandardCursor::Crosshair:
			v = GLFW_CROSSHAIR_CURSOR;
			break;

		case StandardCursor::Hand:
			v = GLFW_HAND_CURSOR;
			break;

		case StandardCursor::HResize:
			v = GLFW_HRESIZE_CURSOR;
			break;

		case StandardCursor::VResize:
			v = GLFW_VRESIZE_CURSOR;
			break;

	}

	GLFWcursor* c = glfwCreateStandardCursor(v);

	if (!c) {

		Log::error("Cursor", "Failed to create standard cursor");
		return false;

	}

	cursors[id] = c;
	return true;

}



void Cursor::destroyCursor(CursorID id) noexcept {

	if (id == DefaultCursorID) {
		return;
	}

	try {

		if (isActiveCursor(id)) {
			restoreDefaultCursor();
		}

		if (cursors.contains(id)) {

			glfwDestroyCursor(cursors[id]);
			cursors.erase(id);

		}

	} catch (const std::exception& e) {
		Log::error("Cursor", "An exception has been thrown during cursor destruction: %s", e.what());
	}

}



void Cursor::destroyAllCursors() noexcept {

	try {

		restoreDefaultCursor();

		for (const auto& [id, cursor] : cursors) {
			glfwDestroyCursor(cursor);
		}

		cursors.clear();

	} catch (const std::exception& e) {
		Log::error("Cursor", "An exception has been thrown during cursor destruction: %s", e.what());
	}

}



double Cursor::getX() const noexcept {

	double x;
	glfwGetCursorPos(window.windowHandle->handle, &x, nullptr);

	return x;

}



double Cursor::getY() const noexcept {

	double y;
	glfwGetCursorPos(window.windowHandle->handle, nullptr, &y);

	return y;

}



Vec2d Cursor::getPosition() const noexcept {

	Vec2d v;
	glfwGetCursorPos(window.windowHandle->handle, &v.x, &v.y);

	return v;

}



void Cursor::setX(double x) noexcept {
	glfwSetCursorPos(window.windowHandle->handle, x, getY());
}



void Cursor::setY(double y) noexcept {
	glfwSetCursorPos(window.windowHandle->handle, getX(), y);
}



void Cursor::setPosition(const Vec2d& pos) noexcept {
	setPosition(pos.x, pos.y);
}



void Cursor::setPosition(double x, double y) noexcept {
	glfwSetCursorPos(window.windowHandle->handle, x, y);
}