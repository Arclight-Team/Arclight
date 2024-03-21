/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputHandler.hpp
 */

#pragma once

#include "KeyTrigger.hpp"
#include "InputEvent.hpp"

#include <functional>


class InputContext;

class InputHandler {

public:

	using CoActionListener      = std::function<bool(KeyAction, double)>;
	using ActionListener        = std::function<bool(KeyAction)>;
	using KeyListener           = std::function<bool(Key, KeyState)>;
	using CharListener          = std::function<bool(KeyChar)>;
	using CursorListener        = std::function<bool(double, double)>;
	using ScrollListener        = std::function<bool(double, double)>;
	using CursorAreaListener    = std::function<bool(bool)>;

	inline InputHandler() noexcept : actionListener(nullptr), coActionListener(nullptr), keyListener(nullptr), charListener(nullptr), cursorListener(nullptr), scrollListener(nullptr), cursorAreaListener(nullptr) {};


	inline void setActionListener(const ActionListener& listener) {
		actionListener = listener;
	}

	inline void setCoActionListener(const CoActionListener& listener) {
		coActionListener = listener;
	}

	inline void setKeyListener(const KeyListener& listener) {
		keyListener = listener;
	}

	inline void setCharListener(const CharListener& listener) {
		charListener = listener;
	}

	inline void setCursorListener(const CursorListener& listener) {
		cursorListener = listener;
	}

	inline void setScrollListener(const ScrollListener& listener) {
		scrollListener = listener;
	}

	inline void setCursorAreaListener(const CursorAreaListener& listener) {
		cursorAreaListener = listener;
	}

private:

	friend class InputContext;

	ActionListener actionListener;
	CoActionListener coActionListener;
	KeyListener keyListener;
	CharListener charListener;
	CursorListener cursorListener;
	ScrollListener scrollListener;
	CursorAreaListener cursorAreaListener;

};