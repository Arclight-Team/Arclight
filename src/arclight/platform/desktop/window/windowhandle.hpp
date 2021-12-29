/*
 *	 Copyright (c) 2021 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 windowhandle.hpp
 */

#pragma once


class Window;
class InputSystem;

struct WindowUserPtr {

	constexpr WindowUserPtr() : window(nullptr), input(nullptr) {};

	Window* window;
	InputSystem* input;

};


struct GLFWwindow;

struct WindowHandle {

	constexpr WindowHandle() : handle(nullptr) {};

	GLFWwindow* handle;
	WindowUserPtr userPtr;

};