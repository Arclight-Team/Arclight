/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputEvent.hpp
 */

#pragma once

#include "KeyConversion.hpp"
#include "Key.hpp"



struct KeyEvent  {

	constexpr KeyEvent(Key physicalKey, Key virtualKey, KeyState state)
		: physicalKey(physicalKey), virtualKey(virtualKey), state(state) {}

	constexpr Key getScancode() const {
		return KeyConversion::physicalKeyToScancode(physicalKey);
	}

	constexpr bool pressed() const {
		return state == KeyState::Pressed || state == KeyState::Held;
	}

	constexpr bool held() const {
		return state == KeyState::Held;
	}

	constexpr bool released() const {
		return state == KeyState::Released;
	}

	Key physicalKey;
	Key virtualKey;
	KeyState state;

};



struct CharEvent {

	constexpr explicit CharEvent(KeyChar character)
		: character(character) {}

	KeyChar character;

};



struct CursorEvent {

	constexpr CursorEvent(double x, double y, bool relative)
		: x(x), y(y), relative(relative) {}

	double x, y;
	bool relative;

};



struct ScrollEvent {

	constexpr ScrollEvent(double x, double y)
		: x(x), y(y) {}

	double x, y;

};



struct CursorAreaEvent {

	constexpr explicit CursorAreaEvent(bool entered)
		: entered(entered) {}

	bool entered;

};



struct ActionEvent {

	constexpr ActionEvent(KeyAction action, u32 ticks)
		: action(action), ticks(ticks) {}

	KeyAction action;
	u32 ticks;

};