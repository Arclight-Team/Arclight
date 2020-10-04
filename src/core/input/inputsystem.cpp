#include "core/input/inputsystem.h"
#include "core/windowhandle.h"
#include "core/window.h"
#include "core/input/inputevent.h"
#include "util/log.h"
#include "util/assert.h"

#include <GLFW/glfw3.h>



InputSystem::InputSystem() {}

InputSystem::InputSystem(const Window& window) {
	connect(window);
}

InputSystem::~InputSystem() {
	disconnect();
}



void InputSystem::connect(const Window& window) {

	if (!window.isOpen()) {
		Log::warn("Input System", "Cannot connect input system to a closed window");
		return;
	}

	disconnect();

	windowHandle = window.getInternalHandle();
	std::shared_ptr<WindowHandle> handle = getWindowHandle();
	
	if (handle) {
		handle->userPtr.input = this;
	} else {
		Log::error("Input System", "Impossible state: Window open with invalid handle");
		return;
	}

	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(handle->handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	glfwSetKeyCallback(handle->handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

		if (action == GLFW_REPEAT) {
			return;
		}

		InputSystem* input = static_cast<WindowHandle*>(glfwGetWindowUserPointer(window))->userPtr.input;
		input->onKeyEvent(KeyEvent(key, action == GLFW_PRESS ? KeyState::Pressed : KeyState::Released));

	});

	glfwSetCharCallback(handle->handle, [](GLFWwindow* window, unsigned int codepoint) {

		InputSystem* input = static_cast<WindowHandle*>(glfwGetWindowUserPointer(window))->userPtr.input;
		input->onCharEvent(CharEvent(codepoint));

	});

	glfwSetCursorPosCallback(handle->handle, [](GLFWwindow* window, double x, double y) {

		InputSystem* input = static_cast<WindowHandle*>(glfwGetWindowUserPointer(window))->userPtr.input;
		input->onCursorEvent(CursorEvent(x, y));

	});

	glfwSetScrollCallback(handle->handle, [](GLFWwindow* window, double x, double y) {

		InputSystem* input = static_cast<WindowHandle*>(glfwGetWindowUserPointer(window))->userPtr.input;
		input->onScrollEvent(ScrollEvent(x, y));

	});

}



void InputSystem::disconnect() {

	if (!connected()) {
		return;
	}

	auto handle = getWindowHandle();
	arc_assert(handle != nullptr, "Handle unexpectedly null");

	glfwSetKeyCallback(handle->handle, nullptr);
	glfwSetCharCallback(handle->handle, nullptr);
	glfwSetCursorPosCallback(handle->handle, nullptr);
	glfwSetScrollCallback(handle->handle, nullptr);

	handle->userPtr.input = nullptr;

	windowHandle.reset();

}


bool InputSystem::connected() const {

	auto handle = getWindowHandle();

	if (handle && handle->userPtr.input) {
		return true;
	}

	return false;

}



void InputSystem::onKeyEvent(const KeyEvent& event) {
	Log::debug("Input System", "Key: %s, Event: %s", glfwGetKeyName(event.getKey(), GLFW_DONT_CARE), event.pressed() ? "Pressed" : "Released");
}



void InputSystem::onCharEvent(const CharEvent& event) {
	Log::debug("Input System", "Char: %c", static_cast<char>(event.getChar()));
}



void InputSystem::onCursorEvent(const CursorEvent& event) {
	Log::debug("Input System", "Cursor: x = %0.3f, y = %0.3f", event.getX(), event.getY());
}



void InputSystem::onScrollEvent(const ScrollEvent& event) {
	Log::debug("Input System", "Scroll: x = %0.3f, y = %0.3f", event.scrollX(), event.scrollY());
}



std::shared_ptr<WindowHandle> InputSystem::getWindowHandle() const {
	return windowHandle.lock();
}