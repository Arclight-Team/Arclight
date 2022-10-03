/*
 *	 Copyright (c) 2022 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 cursor.cpp
 */

#include "rendercursor.hpp"
#include "window.hpp"
#include "image/image.hpp"
#include "util/bits.hpp"
#include "util/log.hpp"

#include <GLFW/glfw3.h>




RenderCursor::RenderCursor(const Window& window) noexcept : window(window), currentID(DefaultCursorID) {}

RenderCursor::~RenderCursor() noexcept {
	destroyAll();
}



void RenderCursor::restoreDefaultCursor() {
	setCurrent(DefaultCursorID);
}



void RenderCursor::setCurrent(CursorID id) {

	if (isActive(id)) {
		return;
	}

	if (cursors.contains(id) || id == DefaultCursorID) {

		auto ptr = window.getInternalHandle().lock();

		if (ptr) {

			glfwSetCursor(window.getInternalHandle().lock()->handle, cursors.contains(id) ? cursors[id] : nullptr);
			currentID = id;

		} else {

			LogW("Cursor").print("Failed to activate cursor with ID = %d", id);

		}


	} else {

		LogW("Cursor").print("Failed to find matching cursor with ID = %d", id);

	}

}



RenderCursor::CursorID RenderCursor::getCurrent() const {
	return currentID;
}



bool RenderCursor::isActive(CursorID id) const {
	return id == currentID;
}



bool RenderCursor::set(CursorID id, const Image<Pixel::RGBA8>& image, const Vec2i& hotspot) {

	if (id == DefaultCursorID) {
		return false;
	}

	GLFWimage img { static_cast<i32>(image.getWidth()), static_cast<i32>(image.getHeight()), const_cast<u8*>(Bits::toByteArray(image.getImageBuffer().data())) };

	GLFWcursor* c = glfwCreateCursor(&img, hotspot.x, hotspot.y);

	if (!c) {

		LogE("Cursor") << "Failed to create image cursor";
		return false;

	}

	cursors[id] = c;
	return true;

}



bool RenderCursor::set(CursorID id, StandardCursor cursor) {

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

		LogE("Cursor") << "Failed to create standard cursor";
		return false;

	}

	cursors[id] = c;
	return true;

}



void RenderCursor::destroy(CursorID id) noexcept {

	if (id == DefaultCursorID) {
		return;
	}

	try {

		if (isActive(id)) {
			restoreDefaultCursor();
		}

		if (cursors.contains(id)) {

			glfwDestroyCursor(cursors[id]);
			cursors.erase(id);

		}

	} catch (const std::exception& e) {
		LogE("Cursor").print("An exception has been thrown during cursor destruction: %s", e.what());
	}

}



void RenderCursor::destroyAll() noexcept {

	try {

		restoreDefaultCursor();

		for (const auto& [id, cursor] : cursors) {
			glfwDestroyCursor(cursor);
		}

		cursors.clear();

	} catch (const std::exception& e) {
		LogE("Cursor").print("An exception has been thrown during cursor destruction: %s", e.what());
	}

}