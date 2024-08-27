/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowHandle.hpp
 */

#pragma once

#include "Desktop/Window/Window.hpp"

#define Rectangle Rectangle_
#include "Common/Win32.hpp"
#undef Rectangle



class WindowHandle {

public:

	using WindowMessageHandlerFunction = std::function<std::optional<LRESULT>(Window& window, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>;
	using InputMessageHandlerFunction = std::function<std::optional<LRESULT>(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>;

	WindowHandle(HWND hwnd, const Vec2ui& viewportSize);
	~WindowHandle() = default;

	constexpr HWND getHWND() const { return hwnd; }

	void setWindowMessageHandler(const WindowMessageHandlerFunction& handler);
	void setInputMessageHandler(const InputMessageHandlerFunction& handler);

	static std::optional<LRESULT> defaultWindowMessageHandler(Window& window, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK onWindowUpdate(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	struct Fullscreen {

		constexpr Fullscreen() : enabled(false), backupRect { 0, 0, 0, 0 }, backupStyle(0), backupExStyle(0) {}

		bool enabled;
		RECT backupRect;
		DWORD backupStyle;
		DWORD backupExStyle;

	};

	friend class Window;

	HWND hwnd;
	Vec2ui viewportSize;
	bool closeRequested;
	Fullscreen fullscreen;
	Vec2ui minSize;
	Vec2ui maxSize;
	bool resizing;
	bool focused;
	bool minimized;
	bool maximized;
	bool hovered;
	bool resizeable;
	bool decorated;
	HICON hicon;
	HICON hiconSm;
	Window::RefreshFunction refreshFunction;
	Window::MoveFunction moveFunction;
	Window::ResizeFunction resizeFunction;
	Window::StateChangeFunction stateChangeFunction;
	Window::DropFunction dropFunction;
	WindowMessageHandlerFunction windowMessageHandlerFunction;
	InputMessageHandlerFunction inputMessageHandlerFunction;

};
