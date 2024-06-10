/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 InputHandler.hpp
 */

#pragma once

#include "InputEvent.hpp"

#include <functional>



class InputContext;

class InputHandler {

public:

	using ActionCallback        = std::function<bool(const ActionEvent&)>;
	using KeyCallback           = std::function<bool(const KeyEvent&)>;
	using CharCallback          = std::function<bool(const CharEvent&)>;
	using CursorCallback        = std::function<bool(const CursorEvent&)>;
	using ScrollCallback        = std::function<bool(const ScrollEvent&)>;
	using CursorAreaCallback    = std::function<bool(const CursorAreaEvent&)>;

	InputHandler() noexcept :
		onActionEvent(nullptr), onKeyEvent(nullptr), onCharEvent(nullptr),
		onCursorEvent(nullptr), onScrollEvent(nullptr), onCursorAreaEvent(nullptr) {};

	ActionCallback onActionEvent;
	ActionCallback onSteadyEvent;
	KeyCallback onKeyEvent;
	CharCallback onCharEvent;
	CursorCallback onCursorEvent;
	ScrollCallback onScrollEvent;
	CursorAreaCallback onCursorAreaEvent;

};