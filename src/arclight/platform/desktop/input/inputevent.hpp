/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 inputevent.hpp
 */

#pragma once

#include "common/types.hpp"



class KeyEvent {

public:

	constexpr KeyEvent(Key key, KeyState state)
		: key(key), state(state) {}


	constexpr Key getKey() const {
		return key;
	}

	constexpr KeyState getKeyState() const {
		return state;
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

private:

	Key key;
	KeyState state;

};



class CharEvent {

public:

	constexpr CharEvent(KeyChar character)
		: character(character) {}


	constexpr KeyChar getChar() const {
		return character;
	}

private:

	KeyChar character;

};



class CursorEvent {

public:

	constexpr CursorEvent(double x, double y)
		: x(x), y(y) {}


	constexpr double getX() const {
		return x;
	}

	constexpr double getY() const {
		return y;
	}

private:

	double x;
	double y;

};



class ScrollEvent {

public:

	constexpr ScrollEvent(double x, double y)
		: x(x), y(y) {}


	constexpr double scrollX() const {
		return x;
	}

	constexpr double scrollY() const {
		return y;
	}

private:

	double x;
	double y;

};



class CursorAreaEvent {

public:

	constexpr CursorAreaEvent(bool entered) : entered(entered) {}

	constexpr bool areaEntered() const {
		return entered;
	}

	constexpr bool areaLeft() const {
		return !entered;
	}

private:

	bool entered;

};