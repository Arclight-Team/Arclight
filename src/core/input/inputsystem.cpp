#include "core/input/inputsystem.h"
#include "core/windowhandle.h"
#include "core/window.h"
#include "core/input/inputevent.h"
#include "util/log.h"
#include "util/assert.h"

#include <algorithm>

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

	glfwSetMouseButtonCallback(handle->handle, [](GLFWwindow* window, int button, int action, int mods) {

		InputSystem* input = static_cast<WindowHandle*>(glfwGetWindowUserPointer(window))->userPtr.input;
		input->onKeyEvent(KeyEvent(button, action == GLFW_PRESS ? KeyState::Pressed : KeyState::Released));

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



void InputSystem::createContext(const std::string& name, u32 priority) {

	u32 index = getContextIndex(name);

	if (index != invalidContext) {
		Log::warn("Input System", "Input context with name '%s' already exists", name.c_str());
		return;
	}

	inputContexts.emplace(inputContexts.begin() + getPriorityInsertIndex(priority), name, priority);

}



void InputSystem::deleteContext(const std::string& name) {

	u32 index = getContextIndex(name);

	if (index == invalidContext) {
		Log::warn("Input System", "Input context with name '%s' doesn't exist", name.c_str());
		return;
	}

	inputContexts.erase(inputContexts.begin() + index);

}



void InputSystem::enableContext(const std::string& name) {

	u32 index = getContextIndex(name);

	if (index == invalidContext) {
		Log::warn("Input System", "Input context with name '%s' doesn't exist", name.c_str());
		return;
	}

	inputContexts[index].enable();

}



void InputSystem::disableContext(const std::string& name) {

	u32 index = getContextIndex(name);

	if (index == invalidContext) {
		Log::warn("Input System", "Input context with name '%s' doesn't exist", name.c_str());
		return;
	}

	inputContexts[index].disable();

}



void InputSystem::onKeyEvent(const KeyEvent& event) {

	Log::debug("Input System", "Key: %s, Event: %s", glfwGetKeyName(event.getKey(), GLFW_DONT_CARE), event.pressed() ? "Pressed" : "Released");

	for (u32 i = 0; i < inputContexts.size(); i++) {

		if (inputContexts[i].onKeyEvent(event)) {
			break;
		}

	}

}



void InputSystem::onCharEvent(const CharEvent& event) {

	Log::debug("Input System", "Char: %c", static_cast<char>(event.getChar()));

	for (u32 i = 0; i < inputContexts.size(); i++) {

		if (inputContexts[i].onCharEvent(event)) {
			break;
		}

	}

}



void InputSystem::onCursorEvent(const CursorEvent& event) {

	Log::debug("Input System", "Cursor: x = %0.3f, y = %0.3f", event.getX(), event.getY());

	for (u32 i = 0; i < inputContexts.size(); i++) {

		if (inputContexts[i].onCursorEvent(event)) {
			break;
		}

	}

}



void InputSystem::onScrollEvent(const ScrollEvent& event) {

	Log::debug("Input System", "Scroll: x = %0.3f, y = %0.3f", event.scrollX(), event.scrollY());

	for (u32 i = 0; i < inputContexts.size(); i++) {

		if (inputContexts[i].onScrollEvent(event)) {
			break;
		}

	}

}



std::shared_ptr<WindowHandle> InputSystem::getWindowHandle() const {
	return windowHandle.lock();
}



u32 InputSystem::getContextIndex(const std::string& name) const {

	for (u32 i = 0; i < inputContexts.size(); i++) {

		if (name == inputContexts.at(i).getContextName()) {
			return i;
		}

	}

	return invalidContext;

}



u32 InputSystem::getPriorityInsertIndex(u32 priority) const {

	for (u32 i = 0; i < inputContexts.size(); i++) {

		if (priority <= inputContexts.at(i).getPriority()) {
			return i;
		}

	}

	return inputContexts.size();

}