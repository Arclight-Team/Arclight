#include "input/inputsystem.h"
#include "input/inputevent.h"
#include "core/windowhandle.h"
#include "core/window.h"
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
	auto handle = getWindowHandle();
	
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

		if (key == GLFW_KEY_UNKNOWN || action == GLFW_REPEAT) {
			return;
		}

		InputSystem* input = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->input;
		input->onKeyEvent(KeyEvent(key, action == GLFW_PRESS ? KeyState::Pressed : KeyState::Released));

	});

	glfwSetCharCallback(handle->handle, [](GLFWwindow* window, unsigned int codepoint) {

		InputSystem* input = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->input;
		input->onCharEvent(CharEvent(codepoint));

	});

	glfwSetCursorPosCallback(handle->handle, [](GLFWwindow* window, double x, double y) {

		InputSystem* input = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->input;
		input->onCursorEvent(CursorEvent(x, y));

	});

	glfwSetScrollCallback(handle->handle, [](GLFWwindow* window, double x, double y) {

		InputSystem* input = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->input;
		input->onScrollEvent(ScrollEvent(x, y));

	});

	glfwSetMouseButtonCallback(handle->handle, [](GLFWwindow* window, int button, int action, int mods) {

		InputSystem* input = static_cast<WindowUserPtr*>(glfwGetWindowUserPointer(window))->input;
		input->onKeyEvent(KeyEvent(button, action == GLFW_PRESS ? KeyState::Pressed : KeyState::Released));

	});

	setupKeyMap();
	eventCounts.resize(keyStates.size(), 0);

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



InputContext& InputSystem::createContext(u32 id) {

	if (inputContexts.contains(id)) {
		Log::warn("Input System", "Input context with ID %d already exists", id);
		return inputContexts[id];
	}

	inputContexts.try_emplace(id);
	return inputContexts[id];

}



void InputSystem::destroyContext(u32 id) {

	if (!inputContexts.contains(id)) {
		Log::warn("Input System", "Input context with ID %d doesn't exist", id);
		return;
	}

	inputContexts.erase(id);

}



void InputSystem::enableContext(u32 id) {

	if (!inputContexts.contains(id)) {
		Log::warn("Input System", "Input context with ID %d doesn't exist", id);
		return;
	}

	inputContexts[id].enable();

}



void InputSystem::disableContext(u32 id) {

	if (!inputContexts.contains(id)) {
		Log::warn("Input System", "Input context with ID %d doesn't exist", id);
		return;
	}

	inputContexts[id].disable();

}



void InputSystem::onKeyEvent(const KeyEvent& event) {

	keyStates[event.getKey()] = event.getKeyState();
	eventCounts[event.getKey()]++;

	for (auto& [id, context] : inputContexts) {

		if (context.onKeyEvent(event, keyStates)) {
			break;
		}

	}

}



void InputSystem::onCharEvent(const CharEvent& event) {

	for (auto& [id, context] : inputContexts) {

		if (context.onCharEvent(event)) {
			break;
		}

	}

}



void InputSystem::onCursorEvent(const CursorEvent& event) {

	for (auto& [id, context] : inputContexts) {

		if (context.onCursorEvent(event)) {
			break;
		}

	}

}



void InputSystem::onScrollEvent(const ScrollEvent& event) {

	for (auto& [id, context] : inputContexts) {

		if (context.onScrollEvent(event)) {
			break;
		}

	}

}



void InputSystem::updateContinuous(u32 ticks) {

	if (!ticks) {
		return;
	}

	for (u32 i : eventCounts) {

		if ((i / 2) > ticks) {

			//Prevents auto-clicks and mouse abuse
			Log::error("Input System", "Detected non-human generated input");
			resetEventCounts();
			return;

		}

	}

	for (auto& [id, context] : inputContexts) {
		
		if (context.onContinuousEvent(ticks, keyStates, eventCounts)) {
			break;
		}

	}

	resetEventCounts();

}



std::shared_ptr<WindowHandle> InputSystem::getWindowHandle() const {
	return windowHandle.lock();
}



void InputSystem::setupKeyMap() {

	auto handle = getWindowHandle();
	arc_assert(handle != nullptr, "Handle unexpectedly null");
	arc_assert(GLFW_KEY_LAST < 512, "GLFW_KEY_LAST exceeds the keycount maximum of 512 keys");
	
	keyStates.resize(GLFW_KEY_LAST + 1, KeyState::Released);

	for (u32 i = GLFW_MOUSE_BUTTON_1; i <= GLFW_MOUSE_BUTTON_LAST; i++) {
		keyStates[i] = glfwGetMouseButton(handle->handle, i) == GLFW_PRESS ? KeyState::Pressed : KeyState::Released;
	}

	for (u32 i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; i++) {
		keyStates[i] = glfwGetKey(handle->handle, i) == GLFW_PRESS ? KeyState::Pressed : KeyState::Released;
	}

}



void InputSystem::resetEventCounts() {

	arc_assert(eventCounts.size(), "Event counts not initialized");

	std::fill(eventCounts.begin(), eventCounts.end(), 0);
	
}