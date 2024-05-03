/*
 *	 Copyright (c) 2024 - Arclight Team
 *
 *	 This file is part of Arclight. All rights reserved.
 *
 *	 WindowHandle.hpp
 */

#pragma once

#include "Window/Window.hpp"
#include "Common/Win32.hpp"



class WindowClass;

class WindowHandle {

public:

    using MessageHandlerFunction = std::function<LRESULT(Window& w, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>;

    WindowHandle(HWND hwnd, const Vec2i& viewport);
    ~WindowHandle() = default;

    static LRESULT defaultMessageHandler(Window& w, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;

    constexpr HWND getHWND() const { return hwnd; }

private:

    static LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) noexcept;
    static void notifyStateChange(Window& w, WindowState state) noexcept;
    static HICON createIcon(const Image<Pixel::RGBA8>& image, int xhot, int yhot, bool icon);

    void setMessageHandlerFunction(const MessageHandlerFunction& function);

    struct Fullscreen {

        bool enabled;
        RECT backupRect;
        DWORD backupStyle;
        DWORD backupExStyle;

    };

    HWND hwnd;
    Vec2i viewportSize;
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
    MessageHandlerFunction messageHandlerFunction;

    friend Window;
    friend WindowClass;
};
