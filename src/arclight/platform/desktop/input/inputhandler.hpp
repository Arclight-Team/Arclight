/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 inputhandler.hpp
 */

#pragma once

#include "keytrigger.hpp"
#include "inputevent.hpp"

#include <functional>


class InputContext;

class InputHandler {

public:

	typedef std::function<bool(KeyAction, double)> CoActionListener;
	typedef std::function<bool(KeyAction)> ActionListener;
	typedef std::function<bool(Key, KeyState)> KeyListener;
	typedef std::function<bool(KeyChar)> CharListener;
	typedef std::function<bool(double, double)> CursorListener;
	typedef std::function<bool(double, double)> ScrollListener;

	inline InputHandler() noexcept : context(nullptr), actionListener(nullptr), coActionListener(nullptr), keyListener(nullptr), charListener(nullptr), cursorListener(nullptr), scrollListener(nullptr) {};

	virtual ~InputHandler();

	InputHandler(const InputHandler& handler) noexcept = delete;
	InputHandler& operator=(const InputHandler& handler) noexcept = delete;
	InputHandler(InputHandler&& handler) noexcept = default;
	InputHandler& operator=(InputHandler&& handler) noexcept = default;

	inline void setActionListener(ActionListener listener) {
		actionListener = listener;
	}

	inline void setCoActionListener(CoActionListener listener) {
		coActionListener = listener;
	}

	inline void setKeyListener(KeyListener listener) {
		keyListener = listener;
	}

	inline void setCharListener(CharListener listener) {
		charListener = listener;
	}

	inline void setCursorListener(CursorListener listener) {
		cursorListener = listener;
	}

	inline void setScrollListener(ScrollListener listener) {
		scrollListener = listener;
	}

private:

	friend class InputContext;

	InputContext* context;
	ActionListener actionListener;
	CoActionListener coActionListener;
	KeyListener keyListener;
	CharListener charListener;
	CursorListener cursorListener;
	ScrollListener scrollListener;

};