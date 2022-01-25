/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 inputsystem.hpp
 */

#pragma once

#include "cursor.hpp"
#include "inputcontext.hpp"

#include <memory>
#include <map>


class Window;
struct WindowHandle;

class KeyEvent;
class CharEvent;
class CursorEvent;
class ScrollEvent;

class InputSystem final {

public:

	InputSystem();
	explicit InputSystem(const Window& window);

	~InputSystem();

	InputSystem(const InputSystem& system) = delete;
	InputSystem& operator=(const InputSystem& system) = delete;	

	void connect(const Window& window);
	void disconnect();

	InputContext& createContext(u32 id);
	void destroyContext(u32 id);
	void enableContext(u32 id);
	void disableContext(u32 id);

	bool connected() const;

	void onKeyEvent(const KeyEvent& event);
	void onCharEvent(const CharEvent& event);
	void onCursorEvent(const CursorEvent& event);
	void onScrollEvent(const ScrollEvent& event);

	void updateContinuous(u32 ticks);

	KeyState getKeyState(Key key) const;
	Scancode getScancode(Key key) const;
	std::string getKeyNameFromKey(Key key) const;
	std::string getKeyNameFromScancode(Scancode code) const;

	Cursor& getCursor();
	const Cursor& getCursor() const;

	void enableHeldEvent();
	void disableHeldEvent();
	bool isHeldEventEnabled() const;

	std::shared_ptr<WindowHandle> getWindowHandle() const;

private:

	void setupKeyMap(bool activeWindow);
	void resetEventCounts();

	Cursor cursor;

	std::weak_ptr<WindowHandle> windowHandle;
	std::map<u32, InputContext> inputContexts;
	std::vector<KeyState> keyStates;
	std::vector<u32> eventCounts;
	bool propagateHeld;

};