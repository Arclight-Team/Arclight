#include "core/input/inputsystem.h"
#include "core/windowhandle.h"
#include "core/window.h"
#include "core/input/inputevent.h"
#include "util/log.h"
#include "util/assert.h"

#include <GLFW/glfw3.h>



InputSystem::InputSystem() {}

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

	glfwSetKeyCallback(handle->handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		InputSystem* input = static_cast<WindowHandle*>(glfwGetWindowUserPointer(window))->userPtr.input;
		input->onKeyEvent(KeyEvent(scancode, KeyState::Pressed, KeyMods::Shift));
	});

}



void InputSystem::disconnect() {

	if (!connected()) {
		return;
	}

	auto handle = getWindowHandle();
	arc_assert(handle != nullptr, "Handle unexpectedly null");

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



std::shared_ptr<WindowHandle> InputSystem::getWindowHandle() const {
	return windowHandle.lock();
}



void InputSystem::onKeyEvent(const KeyEvent& event) {
	Log::debug("Input System", "LOL");
}