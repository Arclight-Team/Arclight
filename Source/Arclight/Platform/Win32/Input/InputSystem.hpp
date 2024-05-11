/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputSystem.hpp
 */

#pragma once

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "InputContext.hpp"

#include <memory>
#include <map>



class Window;
class WindowHandle;

class KeyEvent;
class CharEvent;
class CursorEvent;
class ScrollEvent;

class InputSystem {

public:

	InputSystem();
	explicit InputSystem(const Window& window);

	~InputSystem();

	InputSystem(const InputSystem& system) = delete;
	InputSystem& operator=(const InputSystem& system) = delete;

	InputSystem(InputSystem&& system) noexcept;
	InputSystem& operator=(InputSystem&& system) noexcept;

	bool connect(const Window& window);
	void disconnect();

	InputContext& createContext(u32 id);
	InputContext& getContext(u32 id);
	const InputContext& getContext(u32 id) const;
	void destroyContext(u32 id);
	void enableContext(u32 id);
	void disableContext(u32 id);

	bool connected() const;

	void onKeyEvent(const KeyEvent& event);
	void onCharEvent(const CharEvent& event);
	void onCursorEvent(const CursorEvent& event);
	void onScrollEvent(const ScrollEvent& event);
	void onCursorAreaEvent(const CursorAreaEvent& event);

	void updateContinuous(u32 ticks);

	std::shared_ptr<WindowHandle> getWindowHandle() const;

	void releaseAllKeys();

private:

	bool setupInputDevices();
	void resetEventCounts();

	void setInputPointer();
	void resetInputPointer();

	std::weak_ptr<WindowHandle> windowHandle;
	std::map<u32, InputContext> inputContexts;

	Keyboard keyboard;
	Mouse mouse;

};