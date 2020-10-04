#pragma once

#include "types.h"


typedef u32 Key;

enum class KeyState {
	Pressed,
	Released,
	Repeated
};



enum class KeyMods {
	Shift = 0x1,
	Control = 0x2,
	Alt = 0x4,
	Super = 0x8
};




class KeyEvent {

public:

	constexpr KeyEvent(Key key, KeyState state, KeyMods mods)
		: key(key), state(state), mods(mods) {}


	constexpr Key getKey() const {
		return key;
	}

	constexpr KeyState getKeyState() const {
		return state;
	}

	constexpr bool pressed() const {
		return state == KeyState::Pressed;
	}

	constexpr bool released() const {
		return state == KeyState::Released;
	}

	constexpr bool repeated() const {
		return state == KeyState::Repeated;
	}
	/*
	bool shiftMod() const {

	}
	bool ctrlMod() const;
	bool altMod() const;
	bool superMod() const;
	*/
private:
	Key key;
	KeyState state;
	KeyMods mods;

};