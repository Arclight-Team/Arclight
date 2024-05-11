/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputSystem.cpp
 */

#include "InputSystem.hpp"
#include "InputEvent.hpp"
#include "InputDevice.hpp"
#include "Common/Assert.hpp"
#include "Window/Window.hpp"
#include "Window/WindowHandle.hpp"
#include "Math/Shapes/Rectangle.hpp"
#include "Util/Bool.hpp"
#include "Util/Log.hpp"
#include "Util/Range.hpp"
#include "OS/Common.hpp"

#include "windowsx.h"

#include <algorithm>



InputSystem::InputSystem() {}

InputSystem::InputSystem(const Window& window) {
	connect(window);
}

InputSystem::~InputSystem() {
	disconnect();
}

InputSystem::InputSystem(InputSystem&& system) noexcept :
	windowHandle(std::move(system.windowHandle)),
	inputContexts(std::move(system.inputContexts)) {
	setInputPointer();
}

InputSystem& InputSystem::operator=(InputSystem&& system) noexcept {

	windowHandle = std::move(system.windowHandle);
	inputContexts = std::move(system.inputContexts);

	setInputPointer();

	return *this;

}


bool InputSystem::connect(const Window& window) {

	if (!window.isOpen()) {
		LogW("Input System") << "Cannot connect input system to a closed window";
		return false;
	}

	disconnect();

	windowHandle = window.getInternalHandle();
	auto handle = getWindowHandle();

	if (!handle) {
		LogE("Input System") << "Impossible state: Window open with invalid handle";
		return false;
	}

	setInputPointer();
	bool result = setupInputDevices();

	return result;

}



void InputSystem::disconnect() {

	if (!connected()) {
		return;
	}

	resetInputPointer();
	windowHandle.reset();

}


bool InputSystem::connected() const {

	auto handle = getWindowHandle();
	return !!handle;

}



InputContext& InputSystem::createContext(u32 id) {

	if (inputContexts.contains(id)) {
		LogW("Input System").print("Input context with ID %d already exists", id);
		return inputContexts[id];
	}

	inputContexts.try_emplace(id);
	return inputContexts[id];

}



InputContext& InputSystem::getContext(u32 id) {

	arc_assert(inputContexts.contains(id), "Input context with ID %d doesn't exist", id);
	return inputContexts.at(id);

}



const InputContext& InputSystem::getContext(u32 id) const {

	arc_assert(inputContexts.contains(id), "Input context with ID %d doesn't exist", id);
	return inputContexts.at(id);

}



void InputSystem::destroyContext(u32 id) {

	if (!inputContexts.contains(id)) {
		LogW("Input System").print("Input context with ID %d doesn't exist", id);
		return;
	}

	inputContexts.erase(id);

}



void InputSystem::enableContext(u32 id) {

	if (!inputContexts.contains(id)) {
		LogW("Input System").print("Input context with ID %d doesn't exist", id);
		return;
	}

	inputContexts[id].enable();

}



void InputSystem::disableContext(u32 id) {

	if (!inputContexts.contains(id)) {
		LogW("Input System").print("Input context with ID %d doesn't exist", id);
		return;
	}

	inputContexts[id].disable();

}



void InputSystem::onKeyEvent(const KeyEvent& event) {

	LogI() << "Key " << Log::Hex << " Physical " << event.getPhysicalKey() << " Virtual " << event.getVirtualKey() << " Scancode " << event.getScancode() << " Pressed " << event.pressed();
/*
	keyStates[event.getKey()] = event.getKeyState();
	eventCounts[event.getKey()]++;

	for (auto& [id, context] : inputContexts) {

		if (context.onKeyEvent(event, keyStates)) {
			break;
		}

	}
*/
}



void InputSystem::onCharEvent(const CharEvent& event) {

	// LogI() << "Char " << event.getChar();

	for (auto& [id, context] : inputContexts) {

		if (context.onCharEvent(event)) {
			break;
		}

	}

}



void InputSystem::onCursorEvent(const CursorEvent& event) {

	// LogI() << "Cursor " << event.getX() << ", " << event.getY();

	for (auto& [id, context] : inputContexts) {

		if (context.onCursorEvent(event)) {
			break;
		}

	}

}



void InputSystem::onScrollEvent(const ScrollEvent& event) {

	LogI() << "Scroll X: " << event.scrollX() << ", Y: " << event.scrollY();

	for (auto& [id, context] : inputContexts) {

		if (context.onScrollEvent(event)) {
			break;
		}

	}

}



void InputSystem::onCursorAreaEvent(const CursorAreaEvent& event) {

	LogI() << "Area " << event.areaEntered();

	for (auto& [id, context] : inputContexts) {

		if (context.onCursorAreaEvent(event)) {
			break;
		}

	}

}




void InputSystem::updateContinuous(u32 ticks) {

	if (!ticks) {
		return;
	}
/*
	for (u32 i : eventCounts) {

		if ((i / 2) > ticks) {

			//Prevents auto-clicks and mouse abuse
			LogE("Input System") << "Detected non-human generated input";
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
*/
}



std::shared_ptr<WindowHandle> InputSystem::getWindowHandle() const {
	return windowHandle.lock();
}



void InputSystem::releaseAllKeys() {

	keyboard.releaseAllKeys(*this);
	mouse.releaseAllKeys(*this);

}



bool InputSystem::setupInputDevices() {

	auto handle = windowHandle.lock();

	if (!handle) {
		return false;
	}

	return keyboard.initialize(*this) && mouse.initialize(*this, false);

}



void InputSystem::resetEventCounts() {

	//arc_assert(!eventCounts.empty(), "Event counts not initialized");

	//std::ranges::fill(eventCounts, 0);

}



void InputSystem::setInputPointer() {

	auto handle = windowHandle.lock();

	if (handle) {

		handle->setInputMessageHandler([this](HWND hwnd, UINT messageType, WPARAM wParam, LPARAM lParam) -> std::optional<LRESULT> {

			switch (messageType) {

				case WM_INPUT: {

					RAWINPUT input;
					UINT bufferSize = sizeof(input);
					GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &input, &bufferSize, sizeof(RAWINPUTHEADER));

					switch (input.header.dwType) {

						case RIM_TYPEKEYBOARD:
							keyboard.dispatchInput(*this, &input.data.keyboard);
							break;

						case RIM_TYPEMOUSE:
							mouse.dispatchInput(*this, &input.data.mouse);
							break;

						default:
							break;

					}

				} break;

				case WM_CHAR: {

					char16_t c = wParam;

					if (!Unicode::isSurrogate(c)) {

						onCharEvent(CharEvent(c));

					} else {

						MSG nextMsg;
						LONG msgTime = GetMessageTime();

						if (PeekMessageW(&nextMsg, hwnd, 0, 0, PM_NOREMOVE) && nextMsg.message == WM_CHAR && nextMsg.time == msgTime) {

							char16_t d = nextMsg.wParam;

							if (Unicode::isHighSurrogate(d)) {
								std::swap(c, d);
							}

							Unicode::Codepoint codepoint = Unicode::composeSurrogate(c, d);
							onCharEvent(CharEvent(codepoint));

							PeekMessageW(&nextMsg, hwnd, 0, 0, PM_REMOVE);

						}

					}

				} break;

				case WM_LBUTTONDOWN:
				case WM_LBUTTONUP:
				case WM_MBUTTONDOWN:
				case WM_MBUTTONUP:
				case WM_RBUTTONDOWN:
				case WM_RBUTTONUP:
				case WM_XBUTTONDOWN:
				case WM_XBUTTONUP:
				case WM_MOUSEMOVE:
				case WM_MOUSEWHEEL:
				case WM_MOUSEHWHEEL:
				case WM_MOUSELEAVE: {

					MSG message;
					message.lParam = lParam;
					message.wParam = wParam;
					message.message = messageType;

					mouse.dispatchInput(*this, &message);

				} break;

				case WM_SETFOCUS:
					mouse.setTrapped(mouse.isTrapped());
					break;

				default:
					return {};

			}

			return 0;

		});

	}

}



void InputSystem::resetInputPointer() {

	if (auto handle = windowHandle.lock()) {
		handle->setInputMessageHandler(nullptr);
	}

}
