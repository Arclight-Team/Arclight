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
#include "Util/Log.hpp"
#include "Util/Range.hpp"
#include "OS/Common.hpp"

#include "windowsx.h"

#include <algorithm>
#include <ranges>



InputSystem::InputSystem() = default;

InputSystem::InputSystem(const Window& window) {
	connect(window);
}

InputSystem::~InputSystem() {
	disconnect();
}

InputSystem::InputSystem(InputSystem&& system) noexcept :
	windowHandle(std::move(system.windowHandle)),
	inputContexts(std::move(system.inputContexts)),
	keyboard(system.keyboard),
	mouse(system.mouse),
	keyContext(std::move(system.keyContext)) {

	setInputPointer();

}

InputSystem& InputSystem::operator=(InputSystem&& system) noexcept {

	windowHandle = std::move(system.windowHandle);
	inputContexts = std::move(system.inputContexts);
	keyboard = system.keyboard;
	mouse = system.mouse;
	keyContext = std::move(system.keyContext);

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

	keyContext.setKeyState(event.physicalKey, event.virtualKey, event.pressed());

	for (auto& context : inputContexts | std::views::values) {

		if (context.onKeyEvent(event, keyContext)) {
			break;
		}

	}

}



void InputSystem::onCharEvent(const CharEvent& event) {

	for (auto& context : inputContexts | std::views::values) {

		if (context.onCharEvent(event)) {
			break;
		}

	}

}



void InputSystem::onCursorEvent(const CursorEvent& event) {

	for (auto& context : inputContexts | std::views::values) {

		if (context.onCursorEvent(event)) {
			break;
		}

	}

}



void InputSystem::onScrollEvent(const ScrollEvent& event) {

	for (auto& context : inputContexts | std::views::values) {

		if (context.onScrollEvent(event)) {
			break;
		}

	}

}



void InputSystem::onCursorAreaEvent(const CursorAreaEvent& event) {

	for (auto& context : inputContexts | std::views::values) {

		if (context.onCursorAreaEvent(event)) {
			break;
		}

	}

}




void InputSystem::update(u32 ticks) {

	if (!ticks) {
		return;
	}

	for (auto& context : inputContexts | std::views::values) {

		if (context.onSteadyEvent(ticks, keyContext)) {
			break;
		}

	}

	keyContext.resetEvents();

}



std::shared_ptr<WindowHandle> InputSystem::getWindowHandle() const {
	return windowHandle.lock();
}



void InputSystem::releaseAllKeys() {

	auto pkeys = keyContext.getKeyMap(true);
	auto vkeys = keyContext.getKeyMap(false);

	keyContext.resetAll();

	for (u32 i : Range(pkeys.size())) {

		if (pkeys[i]) {
			onKeyEvent(KeyEvent(i, 0, KeyState::Released));
		}

	}

	for (u32 i : Range(vkeys.size())) {

		if (vkeys[i]) {
			onKeyEvent(KeyEvent(0, i, KeyState::Released));
		}

	}

}



Keyboard& InputSystem::getKeyboard() {
	return keyboard;
}

const Keyboard& InputSystem::getKeyboard() const {
	return keyboard;
}

Mouse& InputSystem::getMouse() {
	return mouse;
}

const Mouse& InputSystem::getMouse() const {
	return mouse;
}



bool InputSystem::setupInputDevices() {

	auto handle = windowHandle.lock();

	if (!handle) {
		return false;
	}

	return keyboard.initialize(*this) && mouse.initialize(*this, false);

}



void InputSystem::setInputPointer() {

	if (auto handle = windowHandle.lock()) {

		handle->setInputMessageHandler([this](HWND hwnd, UINT messageType, WPARAM wParam, LPARAM lParam) -> std::optional<LRESULT> {

			switch (messageType) {

				case WM_INPUT: {

					RAWINPUT input;
					UINT bufferSize = sizeof(input);
					GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, &input, &bufferSize, sizeof(RAWINPUTHEADER));

					switch (input.header.dwType) {

						case RIM_TYPEKEYBOARD:
							keyboard.dispatchInput(*this, keyContext, &input.data.keyboard);
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
